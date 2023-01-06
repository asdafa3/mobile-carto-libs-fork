/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_MBVTBUILDER_MBVTENCODER_H_
#define _CARTO_MBVTBUILDER_MBVTENCODER_H_

#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <variant>

#include <cglib/vec.h>
#include <protobuf/encodedpbf.hpp>

namespace carto { namespace mbvtbuilder {
    class LayerEncoder final {
    public:
        using Point = cglib::vec2<float>;
        using Key = std::string;
        using Value = std::variant<bool, std::string, long long, double>;
        using Property = std::pair<Key, Value>;

        explicit LayerEncoder(const std::string& name, int version = 0, int extent = 0);

        void addMultiPoint(std::uint64_t id, const std::vector<Point>& coords, const std::vector<Property>& properties);
        void addMultiLineString(std::uint64_t id, const std::vector<std::vector<Point>>& coordsList, const std::vector<Property>& properties);
        void addMultiPolygon(std::uint64_t id, const std::vector<std::vector<Point>>& ringsList, const std::vector<Property>& properties);

        protobuf::encoded_message buildLayer() const;

    private:
        inline static constexpr int DEFAULT_LAYER_VERSION = 2;
        inline static constexpr int DEFAULT_LAYER_EXTENT = 4096;

        void importEncodedFeature(std::uint64_t id, int type, const std::vector<std::uint32_t>& geometry, const std::vector<Property>& properties);
        void importProperty(const std::string& key, const Value& value, std::vector<std::uint32_t>& tags);

        static std::vector<std::uint32_t> encodePointCoordinates(const std::vector<Point>& coords, float scale);
        static std::vector<std::uint32_t> encodeLineStringCoordinates(const std::vector<std::vector<Point>>& coordsList, float scale);
        static std::vector<std::uint32_t> encodePolygonRingCoordinates(const std::vector<std::vector<Point>>& ringsList, float scale);

        static void pushZigZagCoords(std::vector<std::uint32_t>& encodedCoords, const cglib::vec2<int>& intCoords, const cglib::vec2<int>& prevIntCoords);

        static protobuf::encoded_message encodeValue(const Value& val);
        static protobuf::encoded_message encodeFeature(std::uint64_t id, int type, const std::vector<uint32_t>& tags, const std::vector<std::uint32_t>& geometry);

        const std::string _name;
        const int _version;
        const int _extent;

        std::vector<std::string> _keys;
        std::unordered_map<std::string, std::size_t> _keyIndexMap;

        std::vector<Value> _values;
        std::unordered_map<std::string, std::size_t> _valueIndexMap;

        std::list<protobuf::encoded_message> _encodedFeatures;
    };
} }

#endif
