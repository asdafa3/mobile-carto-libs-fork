#include <mapsforgereader/utils/TileUtils.h>

namespace carto {
    MapTile TileUtils::CalculateMapTile(const MapPos& mapPos, int zoom) {
        // FIXME: dont hardcode mapbounds 
        MapPos posMin(-PI * EARTH_RADIUS, -PI * EARTH_RADIUS);
        MapPos posMax(PI * EARTH_RADIUS, PI * EARTH_RADIUS);
        MapPos delta = posMax - posMin;
        double tileWidth = delta(0) / (1 << zoom);
        double tileHeight = delta(1) / (1 << zoom);
        MapPos mapVec = mapPos - posMin;
        int x = static_cast<int>(std::floor(mapVec(0) / tileWidth));
        int y = static_cast<int>(std::floor(mapVec(1) / tileHeight));
        return MapTile(zoom, x, y);
    }
}