#include "TorqueCartoCSSMapnikTranslator.h"
#include "mapnikvt/TorqueMarkerSymbolizer.h"

namespace carto::css {
    std::string TorqueCartoCSSMapnikTranslator::getPropertySymbolizerId(const std::string& propertyName) const {
        // Torque uses very simplified CartoCSS with no attachments, etc
        return "marker";
    }

    std::shared_ptr<const mvt::Symbolizer> TorqueCartoCSSMapnikTranslator::createSymbolizer(const std::string& symbolizerType, const std::vector<std::shared_ptr<const Property>>& properties, const std::shared_ptr<mvt::Map>& map) const {
        if (symbolizerType != "marker") {
            _logger->write(mvt::Logger::Severity::ERROR, "Unsupported Torque symbolizer type: " + symbolizerType);
            return std::shared_ptr<mvt::Symbolizer>();
        }
        auto mapnikSymbolizer = std::make_shared<mvt::TorqueMarkerSymbolizer>(_logger);
        for (const std::shared_ptr<const Property>& prop : properties) {
            std::string propertyId = prop->getField().substr(prop->getField().rfind('/') + 1);
            auto it = _symbolizerPropertyMap.find(propertyId);
            if (it == _symbolizerPropertyMap.end()) {
                _logger->write(mvt::Logger::Severity::ERROR, "Unsupported Torque symbolizer property: " + propertyId);
                continue;
            }
            try {
                mvt::Expression mapnikExpression = buildExpression(prop->getExpression());
                if (auto param = mapnikSymbolizer->getParameter(it->second)) {
                    param->setExpression(mapnikExpression);
                }
            }
            catch (const std::exception& ex) {
                _logger->write(mvt::Logger::Severity::ERROR, "Error while setting " + propertyId + " parameter: " + ex.what());
            }
        }
        return mapnikSymbolizer;
    }

    const std::unordered_map<std::string, std::string> TorqueCartoCSSMapnikTranslator::_symbolizerPropertyMap = {
        { "marker-type", "marker-type" },
        { "marker-file", "file" },
        { "marker-fill", "fill" },
        { "marker-fill-opacity", "fill-opacity" },
        { "marker-width", "width" },
        { "marker-opacity", "opacity" },
        { "marker-line-color", "stroke" },
        { "marker-line-opacity", "stroke-opacity" },
        { "marker-line-width", "stroke-width" },
        { "comp-op", "comp-op" } // NOTE: comp-op in Torque is not a layer property, like in normal CartoCSS
    };
}
