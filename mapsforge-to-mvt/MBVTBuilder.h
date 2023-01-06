/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_MBVTBUILDER_MBVTBUILDER_H_
#define _CARTO_MBVTBUILDER_MBVTBUILDER_H_

#include "mapsforge-to-mvt/Types.h"

#include <cstdint>
#include <variant>
#include <vector>
#include <mutex>

#include <boost/math/constants/constants.hpp>

#include <cglib/vec.h>
#include <cglib/bbox.h>

#include <map>
#include <string>

//#include <picojson/picojson.h>

#include <protobuf/encodedpbf.hpp>

namespace carto { namespace mbvtbuilder {
    class LayerEncoder;

    /**
     * Variant of the tile builder component used by the carto-libs/mbvtbuilder library, but changed to allow
     * vector inputs for features instead of GeoJson.
     */
    class MBVTBuilder final {
    public:
        using LayerIndex = int;

        using Bounds = cglib::bbox2<double>;

        using Geometry = carto::mbvt::Geometry_t<double>;
        using PropertyType = carto::mbvt::PropertyType;

        explicit MBVTBuilder(int minZoom, int maxZoom);

        void setFastSimplifyMode(bool enabled);

        std::vector<LayerIndex> getLayerIndices() const;
        Bounds getLayerBounds(LayerIndex layerIndex) const;

        /**
         * Create a new layer for this tile builder instance. One layer is a collection of semantically related features
         * like buildings or streets. Each layer is identified by a name and gets a unique identifier. Further a buffer
         * can be set. This buffer is used in the clipping process of tiles and should reduce visual artifacts by introducing
         * an overlapping of tiles.
         *
         * @param name The name of the layer to create.
         * @param buffer Positive values indicate a buffer in fractional multiples of 256, negative force the default buffer size (5/256).
         * @return Integer identifier of the layer.
         */
        LayerIndex createLayer(const std::string& name, float buffer = -1);

        /**
         * Clears the features and simplification cache of a selected layer.
         *
         * @param layerIndex Integer to identify the layer.
         */
        void clearLayer(int layerIndex);
        void deleteLayer(LayerIndex layerIndex);

        void addMultiPoint(LayerIndex layerIndex, Geometry::MultiPoint coords, std::vector<PropertyType::Property> properties);
        void addMultiLineString(LayerIndex layerIndex, const Geometry::MultiLineString& coordsList, std::vector<PropertyType::Property> properties);
        void addMultiPolygon(LayerIndex layerIndex, const Geometry::MultiPolygon& ringsList, std::vector<PropertyType::Property> properties);

        /**
         * Takes a vector of ways and imports them to a tile builder layer. Classifies ways as either Point, LineString,
         * simple Polygon, or Polygon with holes.
         *
         * Each "way" is a vector< pair< vector <vector <vector <Point>> , vector< Property >>.
         *
         * @param layerIndex The layer to fill.
         * @param features The features to write import
         */
        void importFeatures(LayerIndex layerIndex, const std::vector<Geometry::Way> &features);
        void importFeature(LayerIndex layerIndex, const Geometry::Way &feature);

        /**
         * Builds a protobuf encoded vector tile from the features imported to the layers of the tile builder object.
         * Each layer is visited, simplified (and cached), the features visited and intersection with a bbox derived
         * from the input tileX and tileY coordinates and the zoom level. If features are contained in the bbox,
         * they are clipped to the bbox of the tile (usually with a buffer around the tile) and written to a layer encoder.
         * This layer encoder converts the feature coordinates to integer grid coordinates of a tile and encodes the layer
         * according to the Mapbox vector tile specification.
         *
         * @param zoom The zoom level of the query tile.
         * @param tileX The X integer coordinate of the query tile in the global tiling grid.
         * @param tileY The Y integer coordinate of the query tile in the global tiling grid.
         * @param encodedTile The mvt protobuf tile to write.
         */
        void buildTile(int zoom, int tileX, int tileY, protobuf::encoded_message& encodedTile) const;

    private:
        inline static constexpr float DEFAULT_LAYER_BUFFER = 5.0f / 256.0f;

        struct Feature {
            std::uint64_t id = 0;
            Bounds bounds = Bounds::smallest(); // EPSG3856
            Geometry::Geometry geometry; // EPSG3856
            std::vector<PropertyType::Property> properties;
        };

        struct Layer {
            std::string name;
            Bounds bounds = Bounds::smallest(); // EPSG3856
            std::vector<Feature> features;
            float buffer = 0;
        };

        inline static constexpr double PI = boost::math::constants::pi<double>();
        inline static constexpr double EARTH_RADIUS = 6378137.0;
        inline static constexpr double TILE_TOLERANCE = 1.0 / 256.0;

        const std::map<LayerIndex, Layer>& simplifyAndCacheLayers(int zoom) const;
        void invalidateCache() const;

        static void simplifyLayer(Layer& layer, double tolerance);
        static bool encodeLayer(const Layer& layer, const Geometry::Point& tileOrigin, double tileSize, const Bounds& tileBounds, LayerEncoder& layerEncoder);

        static std::vector<std::vector<Geometry::Point>> parseCoordinatesRings(const std::vector<std::vector<Geometry::Point>> &coords);
        static std::vector<Geometry::Point> parseCoordinatesList(const std::vector<Geometry::Point> &coords);

        /**
         * Parses a point in WGS and converts the datum to meters. This is the format expexted by the layer encoder.
         * Is called from parseCoordinatesRings and parseCoordinatesList method.
         *
         * @param coords Point Coordinates to tranform.
         * @return Point with coordinates in meters.
         */
        static Geometry::Point parseCoordinates(const Geometry::Point &coords);

        static Geometry::Point wgs84ToWM(const Geometry::Point& posWgs84);

        bool _fastSimplifyMode = false;

        std::uint64_t _featureIdCounter = 0;
        std::map<LayerIndex, Layer> _layers;

        const int _minZoom;
        const int _maxZoom;

        mutable std::map<int, std::map<LayerIndex, Layer>> _cachedZoomLayers;

        mutable std::mutex _mutex;
    };
} }

#endif
