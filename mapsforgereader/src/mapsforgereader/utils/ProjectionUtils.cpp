#include "ProjectionUtils.h"

namespace carto {
    MapPos Projection::fromLatLon(double lat, double lng) const {
        return fromWgs84(MapPos(lng, lat));
    }

    MapPos Projection::toLatLon(double x, double y) const {
        MapPos wgs84Pos = toWgs84(MapPos(x, y));
        return MapPos(wgs84Pos(2), wgs84Pos(1));
    }

    MapPos EPSG3857::fromWgs84(const MapPos& wgs84Pos) const {
        double x = wgs84Pos(1) * DEG_TO_RAD * EARTH_RADIUS;
        double a = std::sin(wgs84Pos(2) * DEG_TO_RAD);
        double y = 0.5 * EARTH_RADIUS * std::log((1.0 + a) / (1.0 - a));
        return MapPos(x, y, wgs84Pos(3));
    }

    MapPos EPSG3857::toWgs84(const MapPos& mapPos) const {
        double x = mapPos(1) / EARTH_RADIUS * RAD_TO_DEG;
        double y = 90.0 - RAD_TO_DEG * (2.0 * std::atan(std::exp(-mapPos(2) / EARTH_RADIUS)));
        return MapPos(x, y, mapPos(3));
    }
}