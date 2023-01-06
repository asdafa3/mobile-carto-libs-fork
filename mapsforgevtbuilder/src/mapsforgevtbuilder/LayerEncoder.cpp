#include "LayerEncoder.h"

#include <algorithm>
#include <numeric>

#include "mapnikvt/mbvtpackage/MBVTPackage.pb.h"

namespace carto { namespace mbvtbuilder {
    LayerEncoder::LayerEncoder(const std::string& name, int version, int extent) :
        _name(name),
        _version(version > 0 ? version : DEFAULT_LAYER_VERSION),
        _extent(extent > 0 ? extent : DEFAULT_LAYER_EXTENT)
    {
    }

    void LayerEncoder::addMultiPoint(std::uint64_t id, const std::vector<Point>& coords, const std::vector<Property>& properties) {
        std::vector<std::uint32_t> geometry = encodePointCoordinates(coords, static_cast<float>(_extent));
        importEncodedFeature(id, static_cast<int>(vector_tile::Tile_GeomType_POINT), geometry, properties);
    }
    
    void LayerEncoder::addMultiLineString(std::uint64_t id, const std::vector<std::vector<Point>>& coordsList, const std::vector<Property>& properties) {
        std::vector<std::uint32_t> geometry = encodeLineStringCoordinates(coordsList, static_cast<float>(_extent));
        importEncodedFeature(id, static_cast<int>(vector_tile::Tile_GeomType_LINESTRING), geometry, properties);
    }
    
    void LayerEncoder::addMultiPolygon(std::uint64_t id, const std::vector<std::vector<Point>>& ringsList, const std::vector<Property>& properties) {
        std::vector<std::uint32_t> geometry = encodePolygonRingCoordinates(ringsList, static_cast<float>(_extent));
        importEncodedFeature(id, static_cast<int>(vector_tile::Tile_GeomType_POLYGON), geometry, properties);
    }
    
    protobuf::encoded_message LayerEncoder::buildLayer() const {
        protobuf::encoded_message encodedLayer;

        encodedLayer.write_tag(vector_tile::Tile_Layer::kVersionFieldNumber, protobuf::encoded_message::varint_type);
        encodedLayer.write_uint32(_version);

        encodedLayer.write_tag(vector_tile::Tile_Layer::kExtentFieldNumber, protobuf::encoded_message::varint_type);
        encodedLayer.write_uint32(_extent);

        encodedLayer.write_tag(vector_tile::Tile_Layer::kNameFieldNumber, protobuf::encoded_message::length_type);
        encodedLayer.write_string(_name);

        for (const std::string& key : _keys) {
            encodedLayer.write_tag(vector_tile::Tile_Layer::kKeysFieldNumber, protobuf::encoded_message::length_type);
            encodedLayer.write_string(key);
        }

        for (const Value& value : _values) {
            encodedLayer.write_tag(vector_tile::Tile_Layer::kValuesFieldNumber, protobuf::encoded_message::length_type);
            encodedLayer.write_message(encodeValue(value));
        }

        for (const protobuf::encoded_message& encodedFeature : _encodedFeatures) {
            encodedLayer.write_tag(vector_tile::Tile_Layer::kFeaturesFieldNumber, protobuf::encoded_message::length_type);
            encodedLayer.write_message(encodedFeature);
        }

        return encodedLayer;
    }

    void LayerEncoder::importEncodedFeature(std::uint64_t id, int type, const std::vector<std::uint32_t>& geometry, const std::vector<Property>& properties) {
        if (geometry.empty()) {
            return;
        }

        std::vector<std::uint32_t> tags;
        for (Property property :properties) {
            tags.reserve(properties.size());
            importProperty(property.first, property.second, tags);
        }

        protobuf::encoded_message encodedFeature = encodeFeature(id, type, tags, geometry);
        _encodedFeatures.push_back(std::move(encodedFeature));
    }

    void LayerEncoder::importProperty(const std::string& key, const Value& value, std::vector<std::uint32_t>& tags) {
        struct ValueVisitor {
            explicit ValueVisitor() {}

            std::string operator()(const std::string &value) {
                return value;
            }

            std::string operator()(double value) {
                return std::to_string(value);
            }

            std::string operator()(int64_t value) {
                return std::to_string(value);
            }

            std::string operator()(bool value) {
                return value ? "true" : "false";
            }
        };

        auto it1 = _keyIndexMap.find(key);
        if (it1 == _keyIndexMap.end()) {
            it1 = _keyIndexMap.emplace(key, _keys.size()).first;
            _keys.push_back(key);
        }
        tags.push_back(static_cast<std::uint32_t>(it1->second));

        std::string stringRepresentation = std::visit(ValueVisitor(), value);
        auto it2 = _valueIndexMap.find(stringRepresentation);
        if (it2 == _valueIndexMap.end()) {
            it2 = _valueIndexMap.emplace(stringRepresentation, _values.size()).first;
            _values.push_back(value);
        }
        tags.push_back(static_cast<std::uint32_t>(it2->second));
    }

    std::vector<std::uint32_t> LayerEncoder::encodePointCoordinates(const std::vector<Point>& coords, float scale) {
        std::vector<std::uint32_t> encodedCoords;
        std::size_t vertexCount = coords.size();
        encodedCoords.reserve(vertexCount * 3);
        cglib::vec2<std::int32_t> prevIntCoords(0, 0);
        for (std::size_t i = 0; i < coords.size(); i++) {
            cglib::vec2<std::int32_t> intCoords = cglib::vec2<std::int32_t>::convert(coords[i] * scale);
            encodedCoords.push_back(1 | (1 << 3));
            pushZigZagCoords(encodedCoords, intCoords, prevIntCoords);
            prevIntCoords = intCoords;
        }
        return encodedCoords;
    }

    std::vector<std::uint32_t> LayerEncoder::encodeLineStringCoordinates(const std::vector<std::vector<Point>>& coordsList, float scale) {
        std::vector<std::uint32_t> encodedCoords;
        std::size_t vertexCount = std::accumulate(coordsList.begin(), coordsList.end(), std::size_t(0), [](std::size_t count, const std::vector<Point>& coords) { return count + coords.size(); });
        encodedCoords.reserve(vertexCount * 3);
        cglib::vec2<std::int32_t> prevIntCoords(0, 0);
        for (const std::vector<Point>& coords : coordsList) {
            for (std::size_t i = 0; i < coords.size(); i++) {
                cglib::vec2<std::int32_t> intCoords = cglib::vec2<std::int32_t>::convert(coords[i] * scale);
                if (i > 1 && intCoords == prevIntCoords) {
                    continue;
                }
                encodedCoords.push_back((i == 0 ? 1 : 2) | (1 << 3));
                pushZigZagCoords(encodedCoords, intCoords, prevIntCoords);
                prevIntCoords = intCoords;
            }
        }
        return encodedCoords;
    }

    std::vector<std::uint32_t> LayerEncoder::encodePolygonRingCoordinates(const std::vector<std::vector<Point>>& ringsList, float scale) {
        std::vector<std::uint32_t> encodedCoords;
        std::size_t vertexCount = std::accumulate(ringsList.begin(), ringsList.end(), std::size_t(0), [](std::size_t count, const std::vector<Point>& coords) { return count + coords.size(); });
        encodedCoords.reserve(vertexCount * 3 + ringsList.size());
        cglib::vec2<std::int32_t> prevIntCoords(0, 0);
        for (std::size_t n = 0; n < ringsList.size(); n++) {
            std::vector<Point> coords = ringsList[n];

            double area = 0;
            if (!coords.empty()) {
                for (std::size_t i = 1; i < coords.size(); i++) {
                    area += coords[i - 1](0) * coords[i](1) - coords[i](0) * coords[i - 1](1);
                }
                area += coords.back()(0) * coords.front()(1) - coords.front()(0) * coords.back()(1);
            }
            if ((n > 0) != (area < 0)) {
                std::reverse(coords.begin(), coords.end());
            }

            for (std::size_t i = 0; i < coords.size(); i++) {
                cglib::vec2<std::int32_t> intCoords = cglib::vec2<std::int32_t>::convert(coords[i] * scale);
                if (i > 1 && intCoords == prevIntCoords) {
                    continue;
                }
                encodedCoords.push_back((i == 0 ? 1 : 2) | (1 << 3));
                pushZigZagCoords(encodedCoords, intCoords, prevIntCoords);
                prevIntCoords = intCoords;
            }
            encodedCoords.push_back(7 | (1 << 3));
        }
        return encodedCoords;
    }

    protobuf::encoded_message LayerEncoder::encodeValue(const Value& val) {
        struct ValueVisitor {
            explicit ValueVisitor() { }

            protobuf::encoded_message operator()(std::int64_t value) {
                protobuf::encoded_message encodedValue;
                if (value >= 0) {
                    encodedValue.write_tag(vector_tile::Tile_Value::kUintValueFieldNumber, protobuf::encoded_message::varint_type);
                    encodedValue.write_uint64(value);
                } else {
                    encodedValue.write_tag(vector_tile::Tile_Value::kSintValueFieldNumber, protobuf::encoded_message::varint_type);
                    encodedValue.write_sint64(value);
                }
                return encodedValue;
            }

            protobuf::encoded_message operator()(double value) {
                protobuf::encoded_message encodedValue;
                float floatVal = static_cast<float>(value);
                if (floatVal == value) {
                    encodedValue.write_tag(vector_tile::Tile_Value::kFloatValueFieldNumber, protobuf::encoded_message::fixed32_type);
                    encodedValue.write_float(floatVal);
                } else {
                    encodedValue.write_tag(vector_tile::Tile_Value::kDoubleValueFieldNumber, protobuf::encoded_message::fixed64_type);
                    encodedValue.write_double(value);
                }
                return encodedValue;
            }

            protobuf::encoded_message operator()(const std::string &value) {
                protobuf::encoded_message encodedValue;
                encodedValue.write_tag(vector_tile::Tile_Value::kStringValueFieldNumber, protobuf::encoded_message::length_type);
                encodedValue.write_string(value);
                return encodedValue;
            }

            protobuf::encoded_message operator()(bool value) {
                protobuf::encoded_message encodedValue;
                encodedValue.write_tag(vector_tile::Tile_Value::kBoolValueFieldNumber, protobuf::encoded_message::varint_type);
                encodedValue.write_bool(value);
                return encodedValue;
            }
        };
        return std::visit(ValueVisitor{}, val);
    }

    protobuf::encoded_message LayerEncoder::encodeFeature(std::uint64_t id, int type, const std::vector<uint32_t>& tags, const std::vector<std::uint32_t>& geometry) {
        protobuf::encoded_message encodedFeature;
        
        encodedFeature.write_tag(vector_tile::Tile_Feature::kTypeFieldNumber, protobuf::encoded_message::varint_type);
        encodedFeature.write_uint32(type);
        
        if (id) {
            encodedFeature.write_tag(vector_tile::Tile_Feature::kIdFieldNumber, protobuf::encoded_message::varint_type);
            encodedFeature.write_uint64(id);
        }
        
        protobuf::encoded_message encodedTags;
        for (std::uint32_t tag : tags) {
            encodedTags.write_uint32(tag);
        }
        if (!encodedTags.empty()) {
            encodedFeature.write_tag(vector_tile::Tile_Feature::kTagsFieldNumber, protobuf::encoded_message::length_type);
            encodedFeature.write_message(encodedTags);
        }

        protobuf::encoded_message encodedGeometry;
        for (std::uint32_t geom : geometry) {
            encodedGeometry.write_uint32(geom);
        }
        encodedFeature.write_tag(vector_tile::Tile_Feature::kGeometryFieldNumber, protobuf::encoded_message::length_type);
        encodedFeature.write_message(encodedGeometry);

        return encodedFeature;
    }

    void LayerEncoder::pushZigZagCoords(std::vector<std::uint32_t>& encodedCoords, const cglib::vec2<int>& intCoords, const cglib::vec2<int>& prevIntCoords) {
        int dx = intCoords(0) - prevIntCoords(0);
        int dy = intCoords(1) - prevIntCoords(1);
        encodedCoords.push_back((dx << 1) ^ (dx >> 31));
        encodedCoords.push_back((dy << 1) ^ (dy >> 31));
    }
} }
