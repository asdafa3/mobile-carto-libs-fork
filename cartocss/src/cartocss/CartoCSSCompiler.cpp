#include "CartoCSSCompiler.h"
#include "ExpressionUtils.h"
#include "PredicateUtils.h"

#include <set>

namespace carto { namespace css {
    void CartoCSSCompiler::compileMap(const StyleSheet& styleSheet, std::map<std::string, Value>& mapProperties) const {
        // Build flat property lists
        std::map<std::string, Expression> variableMap;
        PredicateContext context;
        context.expressionContext = _context;
        context.expressionContext.variableMap = &variableMap;

        FilteredPropertyListState state;
        std::list<FilteredPropertyList> propertyLists;
        buildPropertyLists(styleSheet, context, state, propertyLists);

        // Gather map properties from property lists
        for (const FilteredPropertyList& propertyList : propertyLists) {
            if (propertyList.attachment.empty()) {
                PredicateEvaluator predEvaluator(context);

                for (const FilteredProperty& filteredProp : propertyList.properties) {
                    bool unreachableProp = false;
                    for (std::size_t filter : filteredProp.filters) {
                        boost::tribool result = std::visit(predEvaluator, *state.predicates[filter]);
                        if (!result || boost::indeterminate(result)) {
                            unreachableProp = true;
                            break;
                        }
                    }
                    if (unreachableProp) {
                        continue;
                    }

                    std::shared_ptr<const Property> prop = state.properties[filteredProp.property];
                    if (auto val = std::get_if<Value>(&prop->getExpression())) {
                        mapProperties[prop->getField()] = *val;
                    }
                }
            }
        }
    }
    
    void CartoCSSCompiler::compileLayer(const StyleSheet& styleSheet, const std::string& layerName, int minZoom, int maxZoom, std::map<std::pair<int, int>, std::list<AttachmentPropertySets>>& layerZoomAttachments) const {
        // Build flat property lists
        std::map<std::string, Expression> variableMap;
        PredicateContext context;
        context.layerName = layerName;
        context.expressionContext = _context;
        context.expressionContext.variableMap = &variableMap;

        FilteredPropertyListState state;
        std::list<FilteredPropertyList> propertyLists;
        buildPropertyLists(styleSheet, context, state, propertyLists);

        // Sort the properties by decreasing specificity
        for (FilteredPropertyList& propertyList : propertyLists) {
            std::stable_sort(propertyList.properties.begin(), propertyList.properties.end(), [&state](const FilteredProperty& filteredProp1, const FilteredProperty& filteredProp2) {
                return state.properties[filteredProp1.property]->getSpecificity() > state.properties[filteredProp2.property]->getSpecificity();
            });
        }

        // Build layer attachments for each zoom level
        int prevZoom = minZoom, zoom = minZoom;
        std::list<FilteredPropertyList> prevOptimizedPropertyLists;
        std::list<AttachmentPropertySets> prevLayerAttachments;
        for (; zoom < maxZoom; zoom++) {
            std::map<std::string, Value> predefinedFieldMap;
            context.expressionContext.predefinedFieldMap = &predefinedFieldMap;
            (*context.expressionContext.predefinedFieldMap)["zoom"] = Value(static_cast<long long>(zoom));
            PredicateEvaluator predEvaluator(context);

            // Evaluate and optimize property lists
            std::list<FilteredPropertyList> optimizedPropertyLists;
            for (const FilteredPropertyList& propertyList : propertyLists) {
                FilteredPropertyList& optimizedPropertyList = optimizedPropertyLists.emplace_back();
                optimizedPropertyList.attachment = propertyList.attachment;
                optimizedPropertyList.properties.reserve(propertyList.properties.size());
                for (const FilteredProperty& filteredProp : propertyList.properties) {
                    // Check if this property is reachable and remove always true conditions
                    std::vector<std::size_t> optimizedFilters;
                    optimizedFilters.reserve(filteredProp.filters.size());
                    bool unreachableProp = false;
                    for (std::size_t filter : filteredProp.filters) {
                        boost::tribool result = std::visit(predEvaluator, *state.predicates[filter]);
                        if (!result) {
                            unreachableProp = true;
                            break;
                        }
                        if (boost::indeterminate(result)) { // keep only indeterminate filters, ignore always true filters
                            optimizedFilters.push_back(filter);
                        }
                    }
                    if (unreachableProp) {
                        continue;
                    }

                    // Store property with optimized filter list
                    FilteredProperty& optimizedProp = optimizedPropertyList.properties.emplace_back();
                    optimizedProp.property = filteredProp.property;
                    optimizedProp.filters = std::move(optimizedFilters);
                }
            }

            // Check if the property lists changed compared to the previous zoom level
            if (optimizedPropertyLists != prevOptimizedPropertyLists) {
                // Build attachments
                std::list<AttachmentPropertySets> layerAttachments;
                for (const FilteredPropertyList& optimizedPropertyList : optimizedPropertyLists) {
                    buildLayerAttachment(optimizedPropertyList, state, layerAttachments);
                }

                // Check if attachments changed compared to the previous attachments
                if (layerAttachments != prevLayerAttachments) {
                    if (zoom > prevZoom) {
                        layerZoomAttachments[std::make_pair(prevZoom, zoom)] = std::move(prevLayerAttachments);
                    }
                    prevLayerAttachments = std::move(layerAttachments);
                    prevZoom = zoom;
                }

                prevOptimizedPropertyLists = std::move(optimizedPropertyLists);
            }
        }
        if (zoom > prevZoom) {
            layerZoomAttachments[std::make_pair(prevZoom, zoom)] = std::move(prevLayerAttachments);
        }
    }

    void CartoCSSCompiler::buildPropertyLists(const StyleSheet& styleSheet, PredicateContext& context, FilteredPropertyListState& state, std::list<FilteredPropertyList>& propertyLists) const {
        for (const StyleSheet::Element& element : styleSheet.getElements()) {
            if (auto decl = std::get_if<VariableDeclaration>(&element)) {
                if (context.expressionContext.variableMap->find(decl->getVariable()) == context.expressionContext.variableMap->end()) {
                    (*context.expressionContext.variableMap)[decl->getVariable()] = decl->getExpression();
                }
            }
            else if (auto ruleSet = std::get_if<RuleSet>(&element)) {
                buildPropertyList(*ruleSet, context, std::string(), std::vector<std::size_t>(), state, propertyLists);
            }
        }

        ExpressionEvaluator exprEvaluator(context.expressionContext);
        for (FilteredPropertyList& propertyList : propertyLists) {
            for (FilteredProperty& filteredProp : propertyList.properties) {
                std::shared_ptr<const Property> prop = state.properties[filteredProp.property];
                Expression evaluatedExpr = std::visit(exprEvaluator, prop->getExpression());
                if (evaluatedExpr != prop->getExpression()) {
                    filteredProp.property = state.insertProperty(Property(prop->getField(), std::move(evaluatedExpr), prop->getSpecificity()));
                }
            }
        }
    }
    
    void CartoCSSCompiler::buildPropertyList(const RuleSet& ruleSet, const PredicateContext& context, const std::string& existingAttachment, const std::vector<std::size_t>& existingFilters, FilteredPropertyListState& state, std::list<FilteredPropertyList>& propertyLists) const {
        // List of selectors to use
        const std::vector<Selector>* selectors = &ruleSet.getSelectors();
        if (selectors->empty() && !context.layerName.empty()) {
            static const std::vector<Selector> emptySelectorSet { Selector() };
            selectors = &emptySelectorSet;
        }
        PredicateEvaluator predEvaluator(context);

        // Process all selectors
        for (const Selector& selector : *selectors) {
            // Build filters for given selector. Check if the filter list is 'reachable'.
            std::vector<std::size_t> filters = existingFilters;
            std::string attachment = existingAttachment;
            bool unreachableProp = false;
            for (const Predicate& pred : selector.getPredicates()) {
                if (auto attachmentPred = std::get_if<AttachmentPredicate>(&pred)) {
                    attachment += "::" + attachmentPred->getAttachment();
                    continue;
                }
                if (auto layerPred = std::get_if<LayerPredicate>(&pred)) {
                    if (_ignoreLayerPredicates) {
                        continue;
                    }
                }

                boost::tribool result = std::visit(predEvaluator, pred);
                if (!result) {
                    unreachableProp = true;
                    break;
                }
                filters.push_back(state.insertPredicate(pred));
            }
            if (unreachableProp) {
                continue;
            }
            
            // Process block elements
            std::set<std::string> existingBlockFields;
            for (const Block::Element& element : ruleSet.getBlock().getElements()) {
                if (auto decl = std::get_if<PropertyDeclaration>(&element)) {
                    if (existingBlockFields.find(decl->getField()) != existingBlockFields.end()) {
                        continue;
                    }
                    existingBlockFields.insert(decl->getField());
                    
                    // Find property set list for current attachment
                    auto propertyListsIt = std::find_if(propertyLists.begin(), propertyLists.end(), [&attachment](const FilteredPropertyList& propertyList) {
                        return propertyList.attachment == attachment;
                    });
                    if (propertyListsIt == propertyLists.end()) {
                        FilteredPropertyList propertyList;
                        propertyList.attachment = attachment;
                        propertyListsIt = propertyLists.insert(propertyListsIt, propertyList);
                    }
                    
                    // Add property
                    Property::RuleSpecificity specificity = calculateRuleSpecificity(filters, state, decl->getOrder());
                    FilteredProperty& filteredProp = propertyListsIt->properties.emplace_back();
                    filteredProp.property = state.insertProperty(Property(decl->getField(), decl->getExpression(), specificity));
                    filteredProp.filters = filters;
                }
                else if (auto subRuleSet = std::get_if<RuleSet>(&element)) {
                    // Recurse with subrule
                    buildPropertyList(*subRuleSet, context, attachment, filters, state, propertyLists);
                }
            }
        }
    }

    void CartoCSSCompiler::buildLayerAttachment(const FilteredPropertyList& propertyList, const FilteredPropertyListState& state, std::list<AttachmentPropertySets>& layerAttachments) const {
        std::list<PropertySet> propertySets;
        for (const FilteredProperty& filteredProp : propertyList.properties) {
            std::shared_ptr<const Property> prop = state.properties[filteredProp.property];
            std::vector<std::shared_ptr<const Predicate>> propFilters;
            propFilters.reserve(filteredProp.filters.size());
            for (std::size_t filter : filteredProp.filters) {
                propFilters.push_back(state.predicates[filter]);
            }

            for (auto propertySetIt = propertySets.begin(); propertySetIt != propertySets.end(); propertySetIt++) {
                // Check if this attribute is already set for given property set
                if (auto existingProp = propertySetIt->findProperty(prop->getField())) {
                    if (existingProp->getSpecificity() >= prop->getSpecificity()) {
                        continue;
                    }
                }

                // Build new property set by setting the attribute and combining filters
                PropertySet propertySet(*propertySetIt);
                if (!propertySet.mergeFilters(propFilters)) {
                    continue;
                }
                propertySet.insertProperty(prop);

                // Check if the property set is redundant (existing filters already cover it)
                if (std::any_of(propertySets.begin(), propertySetIt, [&propertySet](const PropertySet& existingPropertySet) {
                    return existingPropertySet.covers(propertySet);
                })) {
                    continue;
                }

                // If filters did not change, replace existing filter otherwise we must insert the new filter and keep old one
                if (propertySet.getFilters() == propertySetIt->getFilters()) {
                    *propertySetIt = std::move(propertySet);
                }
                else {
                    propertySets.insert(propertySetIt, std::move(propertySet));
                }
            }

            // Build new property set
            PropertySet propertySet;
            if (!propertySet.mergeFilters(propFilters)) {
                continue;
            }
            propertySet.insertProperty(prop);

            // Check if the property set is redundant (existing filters already cover it)
            if (std::any_of(propertySets.begin(), propertySets.end(), [&propertySet](const PropertySet& existingPropertySet) {
                return existingPropertySet.covers(propertySet);
            })) {
                continue;
            }

            // Add the built property set to the list
            propertySets.push_back(std::move(propertySet));
        }

        // Add layer attachment
        layerAttachments.push_back(AttachmentPropertySets(propertyList.attachment, std::move(propertySets)));
    }

    Property::RuleSpecificity CartoCSSCompiler::calculateRuleSpecificity(const std::vector<std::size_t>& filters, const FilteredPropertyListState& state, int order) {
        struct PredicateCounter {
            void operator() (const MapPredicate&) { }
            void operator() (const LayerPredicate&) { layers++; }
            void operator() (const ClassPredicate&) { classes++; }
            void operator() (const AttachmentPredicate&) { }
            void operator() (const OpPredicate&) { filters++; }
            void operator() (const WhenPredicate&) { filters++; }

            int layers = 0;
            int classes = 0;
            int filters = 0;
        };

        PredicateCounter counter;
        for (std::size_t filter : filters) {
            std::visit(counter, *state.predicates[filter]);
        }
        return std::make_tuple(counter.layers, counter.classes, counter.filters, order);
    }
} }
