#include "MBVTBuilder.h"
#include "LayerEncoder.h"
#include "Clipper.h"
#include "Simplifier.h"

#include <functional>
#include <algorithm>
#include <stdexcept>
#include <math.h>
#include <iostream>
#include <string.h>

#include "mapnikvt/mbvtpackage/MBVTPackage.pb.h"

namespace carto { namespace mbvtbuilder {
    MBVTBuilder::MBVTBuilder(int minZoom, int maxZoom) :
    _minZoom(minZoom), _maxZoom(maxZoom)
    {
    }

    void MBVTBuilder::setFastSimplifyMode(bool enabled) {
        std::lock_guard<std::mutex> lock(_mutex);
        _fastSimplifyMode = enabled;
        invalidateCache();
    }

    std::vector<MBVTBuilder::LayerIndex> MBVTBuilder::getLayerIndices() const {
        std::vector<LayerIndex> layerIndices;
        for (const auto & _layer : _layers) {
            layerIndices.push_back(_layer.first);
        }
        return layerIndices;
    }

    MBVTBuilder::LayerIndex MBVTBuilder::createLayer(const std::string& name, float buffer) {
        std::lock_guard<std::mutex> lock(_mutex);
        Layer layer;
        layer.name = name;
        layer.buffer = (buffer >= 0 ? buffer : DEFAULT_LAYER_BUFFER);
        LayerIndex layerIndex = (_layers.empty() ? 0 : _layers.rbegin()->first) + 1;
        _layers.emplace(layerIndex, std::move(layer));
        invalidateCache();
        return layerIndex;
    }

    MBVTBuilder::Bounds MBVTBuilder::getLayerBounds(LayerIndex layerIndex) const {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _layers.find(layerIndex);
        if (it == _layers.end()) {
            throw std::runtime_error("Invalid layer index");
        }
        return it->second.bounds;
    }

    void MBVTBuilder::clearLayer(LayerIndex layerIndex) {
        std::lock_guard<std::mutex> lock(_mutex);
        _layers[layerIndex].bounds = Bounds::smallest();
        _layers[layerIndex].features.clear();
        invalidateCache();
    }

    void MBVTBuilder::deleteLayer(LayerIndex layerIndex) {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _layers.find(layerIndex);
        if (it == _layers.end()) {
            throw std::runtime_error("Invalid layer index");
        }
        _layers.erase(it);
        invalidateCache();
    }

    void MBVTBuilder::addMultiPoint(LayerIndex layerIndex, Geometry::MultiPoint coords, std::vector<PropertyType::Property> properties) {
        Bounds bounds = Bounds::make_union(coords.begin(), coords.end());

        std::lock_guard<std::mutex> lock(_mutex);
        Feature feature;
        feature.id = ++_featureIdCounter;
        feature.bounds = bounds;
        feature.geometry = std::move(coords);
        feature.properties.swap(properties);
        properties.clear();
        
        _layers[layerIndex].bounds.add(feature.bounds);
        _layers[layerIndex].features.push_back(feature);
        invalidateCache();
    }

    void MBVTBuilder::addMultiLineString(LayerIndex layerIndex, const Geometry::MultiLineString& coordsList, std::vector<PropertyType::Property> properties) {
        Bounds bounds = Bounds::smallest();
        for (const std::vector<Geometry::Point>& coords : coordsList) {
            bounds.add(Bounds::make_union(coords.begin(), coords.end()));
        }

        std::lock_guard<std::mutex> lock(_mutex);
        Feature feature;
        feature.id = ++_featureIdCounter;
        feature.bounds = bounds;
        feature.geometry = coordsList;
        feature.properties.swap(properties);
        properties.clear();

        _layers[layerIndex].bounds.add(feature.bounds);
        _layers[layerIndex].features.push_back(feature);
        invalidateCache();
    }

    void MBVTBuilder::addMultiPolygon(LayerIndex layerIndex, const Geometry::MultiPolygon& ringsList, std::vector<PropertyType::Property> properties) {
        Bounds bounds = Bounds::smallest();
        for (const std::vector<std::vector<Geometry::Point>>& rings : ringsList) {
            for (const std::vector<Geometry::Point>& ring : rings) {
                bounds.add(Bounds::make_union(ring.begin(), ring.end()));
            }
        }

        std::lock_guard<std::mutex> lock(_mutex);
        Feature feature;
        feature.id = ++_featureIdCounter;
        feature.bounds = bounds;
        feature.geometry = ringsList;
        feature.properties.swap(properties);
        properties.clear();
        
        _layers[layerIndex].bounds.add(feature.bounds);
        _layers[layerIndex].features.push_back(feature);
        invalidateCache();
    }

    void MBVTBuilder::importFeatures(LayerIndex layerIndex, const std::vector<Geometry::Way> &features) {
        for (const Geometry::Way& feature : features) {
            importFeature(layerIndex, feature);
        }
    }

    void MBVTBuilder::importFeature(LayerIndex layerIndex, const Geometry::Way &feature) {

        Geometry::MultiPolygon coordsDef = feature.first;

        for (auto const &wayBlock : coordsDef) { // In case of Geometry collections
            bool isPoint = wayBlock.size() == 1 && wayBlock[0].size() == 1;
            // store as point if each dimension has size one
            if (isPoint) {
                addMultiPoint(layerIndex, {parseCoordinates(wayBlock[0][0])}, feature.second);
            } else {
                // can be a LineString, simple polygon or nested polygon
                bool isClosed = true;
                bool isComplexGeometry = wayBlock.size() > 1;
                // check if all ways are closed for nested polygons
                for (const auto & i : wayBlock) {
                    // get coordinates from first and last point of coordinate block
                    Geometry::Point firstBlockCoord = i[0];
                    Geometry::Point lastBlockCoord = i[i.size() - 1];
                    // are the inner polygons also closed (first ~ last)?
                    // compare against an epsilon due to numeric precision issues
                    isClosed = isClosed && (fabs(firstBlockCoord[0] - lastBlockCoord[0]) < 8e-10) && (fabs(firstBlockCoord[1] - lastBlockCoord[1]) < 8e-10);
                }

                // geometry can only be a multi polygon if it is closed and has multiple coordinate blocks
                if (isComplexGeometry && isClosed) {
                    std::vector<std::vector<std::vector<Geometry::Point>>> ringsList;
                    for (auto const &way : wayBlock) {
                        ringsList.push_back( { parseCoordinatesRings({ way }) } );
                    }
                    // import as polygon with holes
                    addMultiPolygon(layerIndex, ringsList, feature.second);
                } else {
                    // import as simple polygon
                    if (isClosed) {
                        addMultiPolygon(layerIndex, { parseCoordinatesRings(wayBlock) }, feature.second);
                    // import as line string
                    } else {
                        addMultiLineString(layerIndex, { parseCoordinatesList(wayBlock[0]) }, feature.second);
                    }
                }
            }
        }
    }

    std::vector<std::vector<MBVTBuilder::Geometry::Point>> MBVTBuilder::parseCoordinatesRings(const std::vector<std::vector<Geometry::Point>> &coords) {
        std::vector<std::vector<Geometry::Point>> rings;
        rings.reserve(coords.size());
        for (const auto & coord : coords) {
            std::vector<Geometry::Point> coordList = parseCoordinatesList(coord);
            rings.push_back(coordList);
        }
        return rings;
    }

    std::vector<MBVTBuilder::Geometry::Point> MBVTBuilder::parseCoordinatesList(const std::vector<Geometry::Point> &coords) {
        std::vector<Geometry::Point> coordsList;
        coordsList.reserve(coords.size());
        for (auto coord : coords) {
            Geometry::Point point = parseCoordinates(coord);
            coordsList.push_back(point);
        }
        return coordsList;
    }

    MBVTBuilder::Geometry::Point MBVTBuilder::parseCoordinates(const Geometry::Point &coords) {
        return wgs84ToWM(coords);
    }

    void MBVTBuilder::buildTile(int zoom, int tileX, int tileY, protobuf::encoded_message& encodedTile) const {
        static const Bounds mapBounds(Geometry::Point(-PI * EARTH_RADIUS, -PI * EARTH_RADIUS), Geometry::Point(PI * EARTH_RADIUS, PI * EARTH_RADIUS));

        std::lock_guard<std::mutex> lock(_mutex);

        // performs simplification on entire layers (Douglas Peucker) and caches the results for further buildTile calls
        const std::map<LayerIndex, Layer>& layers = simplifyAndCacheLayers(zoom);
        for (const auto & it : layers) {
            const Layer& layer = it.second;
            if (layer.features.empty()) {
                continue;
            }

            double tileSize = (mapBounds.max(0) - mapBounds.min(0)) / (1 << zoom);
            Geometry::Point tileOrigin(tileX * tileSize + mapBounds.min(0), tileY * tileSize + mapBounds.min(1));

            // add a buffer to the tile bounds to have some overlapping of tiles to reduce visual artefacts
            Bounds tileBounds(tileOrigin - Geometry::Point(layer.buffer, layer.buffer) * tileSize, tileOrigin + Geometry::Point(1 + layer.buffer, 1 + layer.buffer) * tileSize);

            LayerEncoder layerEncoder(layer.name);

            // write features of the layer that are in the tileBounds to layerEncoder that returns a protobuf encoded vector tile in mapbox format
            if (encodeLayer(layer, tileOrigin, tileSize, tileBounds, layerEncoder)) {
                encodedTile.write_tag(vector_tile::Tile::kLayersFieldNumber, protobuf::encoded_message::length_type);
                // build pbf tile from layer features
                encodedTile.write_message(layerEncoder.buildLayer());
            }
        }
    }


    const std::map<MBVTBuilder::LayerIndex, MBVTBuilder::Layer>& MBVTBuilder::simplifyAndCacheLayers(int zoom) const {
        auto it = _cachedZoomLayers.lower_bound(zoom);
        int nextZoom = (it != _cachedZoomLayers.end() ? it->first : _maxZoom + 1);
        while (nextZoom > zoom) {
            int currentZoom = (!_fastSimplifyMode ? zoom : nextZoom - 1);
            _cachedZoomLayers[currentZoom] = (!_fastSimplifyMode || nextZoom > _maxZoom ? _layers : _cachedZoomLayers[nextZoom]);
            std::map<LayerIndex, Layer>& layers = _cachedZoomLayers[currentZoom];
            for (auto & lit : layers) {
                Layer& layer = lit.second;
                double tolerance = 2.0 * PI * EARTH_RADIUS / (1 << currentZoom) * TILE_TOLERANCE;
                simplifyLayer(layer, tolerance);
            }
            nextZoom = currentZoom;
        }
        return _cachedZoomLayers[zoom];
    }

    void MBVTBuilder::invalidateCache() const {
        _cachedZoomLayers.clear();
    }

    void MBVTBuilder::simplifyLayer(Layer& layer, double tolerance) {
        struct GeometryVisitor {
            explicit GeometryVisitor(double tolerance) : _simplifier(tolerance) { }

            void operator() (Geometry::MultiPoint& coords) {
            }

            void operator() (Geometry::MultiLineString& coordsList) {
                for (std::vector<Geometry::Point>& coords : coordsList) {
                    coords = _simplifier.simplifyLineString(coords);
                }
            }

            void operator() (Geometry::MultiPolygon& ringsList) {
                for (std::vector<std::vector<Geometry::Point>>& rings : ringsList) {
                    for (auto it = rings.begin(); it != rings.end(); ) {
                        std::vector<Geometry::Point> simplifiedRing = _simplifier.simplifyPolygonRing(*it);
                        if (simplifiedRing.size() >= 3) {
                            *it++ = simplifiedRing;
                        } else {
                            // delete polygon if it contains less then 3 vertices after simplification to maintain topology
                            it = rings.erase(it);
                        }
                    }
                }
            }

        private:
            const Simplifier<double> _simplifier;
        };

        for (Feature& feature : layer.features) {
            GeometryVisitor visitor(tolerance);
            std::visit(visitor, feature.geometry);
        }
    }

    bool MBVTBuilder::encodeLayer(const Layer& layer, const Geometry::Point& tileOrigin, double tileSize, const Bounds& tileBounds, LayerEncoder& layerEncoder) {
        struct GeometryVisitor {
            explicit GeometryVisitor(const Geometry::Point& tileOrigin, double tileSize, const Bounds& tileBounds, std::uint64_t id, const std::vector<PropertyType::Property>& properties, LayerEncoder& layerEncoder) : _tileOrigin(tileOrigin), _tileScale(1.0 / tileSize), _clipper(tileBounds), _id(id), _properties(properties), _layerEncoder(layerEncoder), _tileSize(tileSize) { }

            bool operator() (const Geometry::MultiPoint& coords) {
                std::vector<mbvt::Geometry_t<float>::Point> tileCoords;
                tileCoords.reserve(coords.size());
                for (const Geometry::Point& pos : coords) {
                    if (_clipper.testPoint(pos)) {
                        tileCoords.push_back(mbvt::Geometry_t<float>::Point::convert((pos - _tileOrigin) * _tileScale));
                    }
                }

                // fill layer encoder
                _layerEncoder.addMultiPoint(_id, tileCoords, _properties);
                return !tileCoords.empty();
            }

            bool operator() (const Geometry::MultiLineString& coordsList) {
                std::vector<std::vector<mbvt::Geometry_t<float>::Point>> tileCoordsList;
                tileCoordsList.reserve(coordsList.size());
                for (const std::vector<Geometry::Point>& coords : coordsList) {
                    // perform geometry clipping for line string
                    std::vector<std::vector<Geometry::Point>> clippedCoordsList = _clipper.clipLineString(coords);
                    for (const std::vector<Geometry::Point>& clippedCoords : clippedCoordsList) {
                        std::vector<mbvt::Geometry_t<float>::Point> tileCoords;
                        tileCoords.reserve(clippedCoords.size());
                        for (const Geometry::Point& pos : clippedCoords) {
                            tileCoords.push_back(mbvt::Geometry_t<float>::Point::convert((pos - _tileOrigin) * _tileScale));
                        }
                        tileCoordsList.push_back(tileCoords);
                    }
                }

                // fill layer encoder
                _layerEncoder.addMultiLineString(_id, tileCoordsList, _properties);
                return !tileCoordsList.empty();
            }

            bool operator() (const Geometry::MultiPolygon& ringsList) {
                std::vector<std::vector<mbvt::Geometry_t<float>::Point>> tileCoordsList;
                for (const std::vector<std::vector<Geometry::Point>>& rings : ringsList) {
                    for (std::size_t i = 0; i < rings.size(); i++) {

                        // perform geometry clipping of rings
                        std::vector<Geometry::Point> clippedRing = _clipper.clipPolygonRing(rings[i]);
                        if (clippedRing.size() < 3) {
                            continue;
                        }

                        std::vector<mbvt::Geometry_t<float>::Point> tileCoords;
                        tileCoords.reserve(clippedRing.size());
                        double signedArea = 0;
                        Geometry::Point prevPos = clippedRing.back();
                        for (const Geometry::Point& pos : clippedRing) {
                            tileCoords.push_back(mbvt::Geometry_t<float>::Point::convert((pos - _tileOrigin) * _tileScale));
                            signedArea += (prevPos(0) - _tileOrigin(0)) * (pos(1) - _tileOrigin(1)) - (prevPos(1) - _tileOrigin(1)) * (pos(0) - _tileOrigin(0));
                            prevPos = pos;
                        }

                        // reverse winding order if of polygon if signed area is < 0 (marks inner polygons)
                        if ((signedArea < 0) != (i == 0)) {
                            std::reverse(tileCoords.begin(), tileCoords.end());
                        }
                        tileCoordsList.push_back(tileCoords);
                    }
                }
                // fill layer encoder
                _layerEncoder.addMultiPolygon(_id, tileCoordsList, _properties);
                return !tileCoordsList.empty();
            }

        private:
            /**
             * Origin of the vector tile to build.
             */
            const Geometry::Point _tileOrigin;

            /**
             * Scale of the tile in meters.
             */
            const double _tileScale;

            /**
             * The tile size in pixels, set to 256.
             */
            const double _tileSize;

            /**
             * The component that clips the layer to the requested tile bounds.
             */
            const Clipper<double> _clipper;

            /**
             * Unique id for each feature.
             */
            const std::uint64_t _id;

            /**
             * The properties contained used by features relevant for the tile to build.
             */
            const std::vector<PropertyType::Property>& _properties;

            /**
             * Component that builds the vector tiles in mvt pbf format.
             */
            LayerEncoder& _layerEncoder;
        };

        bool featuresAdded = false;

        // visit all features contained in the layer
        for (const Feature& feature : layer.features) {

            // check if features is inside layer
            if (!tileBounds.inside(feature.bounds)) {
                continue;
            }

            // perform clipping, check winding order in case of polygons and write features to layerEncoder
            // to built the layer later on
            GeometryVisitor visitor(tileOrigin, tileSize, tileBounds, feature.id, feature.properties, layerEncoder);
            if (std::visit(visitor, feature.geometry)) {
                featuresAdded = true;
            }
        }

        // true to signalize that the layer can be built as it is not empty
        return featuresAdded;
    }

    MBVTBuilder::Geometry::Point MBVTBuilder::wgs84ToWM(const Geometry::Point& posWgs84) {
        double x = EARTH_RADIUS * posWgs84(0) * PI / 180.0;
        double a = posWgs84(1) * PI / 180.0;
        double y = 0.5 * EARTH_RADIUS * std::log((1.0 + std::sin(a)) / (1.0 - std::sin(a)));
        return Geometry::Point(x, -y); // NOTE: we use EPSG3857 with flipped Y
    }
} }
