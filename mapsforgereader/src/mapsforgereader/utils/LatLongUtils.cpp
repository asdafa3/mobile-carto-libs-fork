/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014 Ludwig M Brinckmann
 * Copyright 2014 Christian Pesch
 * Copyright 2014-2017 devemux86
 * Copyright 2015 Andreas Schildbach
 *
 * This program is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LatLongUtils.h"
#include "components/Exceptions.h"
#include "utils/Const.h"
#include "utils/Log.h"
#include <mapsforgereader/MFConstants.h>

#include <math.h>
#include <cmath>

/**
 * 0:=z
 * 1:=x
 * 2:=y
 */


namespace carto {
    double LatLongUtils::microdegreesToDegrees(int coordinate) {
        double value = coordinate / MFConstants::_CONVERSION_FACTOR;
        return value;
    }

    mvt::MapBounds LatLongUtils::enlarge(const mvt::MapBounds& bounds, int meters) {
        if (meters < 0) {
            throw GenericException("mvt::MapBounds::enlarge: BoundingBox extend operation does not accept negative values");
        }

        double verticalExpansion = latitudeDistance(meters);
        double horizontalExpansion = longitudeDistance(meters, std::max(std::abs(bounds.min(2)), std::abs(bounds.max(2))));

        double minLat = std::max(MFConstants::_LATITUDE_MIN, bounds.min(2) - verticalExpansion);
        double minLon = std::max(-180.0, bounds.min(1) - horizontalExpansion);
        double maxLat = std::min(MFConstants::_LATITUDE_MAX, bounds.max(2) + verticalExpansion);
        double maxLon = std::min(180.0, bounds.max(1) + horizontalExpansion);

        MapPos min(minLon, minLat);
        MapPos max(maxLon, maxLat);
        mvt::MapBounds newBounds(min, max);
        return newBounds;
    }

    bool LatLongUtils::intersectsArea(const mvt::MapBounds &mapBounds, const std::vector<std::vector<MapPos>> &wayBlocks) {

        // x is latitude, y is longitude

        if (wayBlocks.empty() || wayBlocks[0].empty()) {
            return false;
        }

        for (auto const &coordinateBlock : wayBlocks) {
            for (auto const &coordinate : coordinateBlock) {
                if (mapBounds.inside(coordinate)) {
                    // if any of the points is inside the map bounds return early
                    return true;
                }
            }
        }

        // no fast exit
        double tmpMinLat = wayBlocks[0][0](2);
        double tmpMinLon = wayBlocks[0][0](1);
        double tmpMaxLat = wayBlocks[0][0](2);
        double tmpMaxLon = wayBlocks[0][0](1);

        // calculate the maximum extent
        for (auto const &outer : wayBlocks) {
            for (auto const &coord : outer) {
                tmpMinLat = std::min(tmpMinLat, coord(2));
                tmpMaxLat = std::max(tmpMaxLat, coord(2));
                tmpMinLon = std::min(tmpMinLon, coord(1));
                tmpMaxLon = std::max(tmpMaxLon, coord(1));
            }
        }

        MapPos min(tmpMinLon, tmpMinLat);
        MapPos max(tmpMaxLon, tmpMaxLat);
        mvt::MapBounds checkBox(min, max);

        // perform the intersection check
        mvt::MapBounds intersectionBBox = mapBounds.intersect(checkBox);
        bool intersects = !intersectionBBox.empty();
        return intersects;
    }

    double LatLongUtils::latitudeDistance(uint32_t meters) {
        return (meters * 360) / (2 * Const::PI * Const::EARTH_RADIUS);
    }

    double LatLongUtils::longitudeDistance(uint32_t meters, double lat) {
        return (meters * 360) / (2 * Const::PI * Const::EARTH_RADIUS * std::cos((lat * Const::PI) / 180));
    }
}
