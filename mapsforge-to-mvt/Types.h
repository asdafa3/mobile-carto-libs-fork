//
// Created by Linus Schwarz on 20.07.21.
//

#ifndef MOBILE_SDK_TYPES_H
#define MOBILE_SDK_TYPES_H

#include <cglib/vec.h>
#include <vector>
#include <variant>
#include <type_traits>

namespace carto {
    namespace mbvt {
        /**
         * Generic Property (Tag) structure uses a string as key and bool, string, long int, or double as values.
         * Provides non spatial information to geometries.
         */
        struct PropertyType {
            /**
             * String key.
             */
            typedef std::string Key;

            /**
             * Variant value.
             */
            typedef std::variant<bool, std::string, long long, double> Value;

            /**
             * Property as tuple of key and value to provide semantic information to geometries.
             */
            typedef std::pair<Key, Value> Property;
        };

        /**
         * Generic geometry type struct that can be used with different numeric base values as floats and doubles
         * and integers, as long as the types are arithmetic types. Implemented as such to be used in different places
         * with different types.
         *
         * @tparam Type The value type to use as base type for geometry coordinates.
         */
        template <typename Type, typename = typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type>
        struct Geometry_t {

            /**
             * A point on the map.
             */
            typedef cglib::vec2<Type> Point;

            /**
             * Default container for point geometries.
             */
            typedef std::vector<Point> MultiPoint;

            /**
             * Container used for line string geometries.
             */
            typedef std::vector<std::vector<Point>> MultiLineString;

            /**
             * Remark: Multipolygon is misnamed here and marks the container for all points, ways and nested ways.
             * This is the datastructure in which geometries are imported. Those are later translated into MultiPoint,
             * MultiLineString and MultiPolygon during the import process.
             *
             * This should be refactored.
             */
            typedef std::vector<std::vector<std::vector<Point>>> MultiPolygon;

            /**
             * Coordinates of import features.
             *
             * This should be renamed.
             */
            typedef MultiPolygon CoordsDef;

            /**
             * A variant type for Geometries that can either hold a MultiPoint, MultiLineString or Multipolygon
             * geometry.
             */
            typedef std::variant<MultiPoint, MultiLineString, MultiPolygon> Geometry;

            /**
             * Definition of a geometry container along with feature attributes.
             */
            typedef std::pair<MultiPolygon, std::vector<PropertyType::Property>> Way;
        };
    }
}

#endif //MOBILE_SDK_TYPES_H
