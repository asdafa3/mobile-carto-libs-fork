#include "ProjectionUtils.h"

namespace carto {
    mvt::MapPos EPSG3857::fromLatLon(const MapPos& pos) const {
        double x = wgs84Pos.getX() * Const::DEG_TO_RAD * EARTH_RADIUS;
        double a = std::sin(wgs84Pos.getY() * Const::DEG_TO_RAD);
        double y = 0.5 * EARTH_RADIUS * std::log((1.0 + a) / (1.0 - a));
        return MapPos(x, y, wgs84Pos.getZ());
    }

    mvt::MapPos EPSG3857::toLatLon(const MapPos& pos) const {
        double x = mapPos.getX() / EARTH_RADIUS * Const::RAD_TO_DEG;
        double y = 90.0 - Const::RAD_TO_DEG * (2.0 * std::atan(std::exp(-mapPos.getY() / EARTH_RADIUS)));
        return MapPos(x, y, mapPos.getZ());
    }
}