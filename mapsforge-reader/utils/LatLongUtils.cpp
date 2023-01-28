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
#include "mapsforge-reader/MFConstants.h"

#include <math.h>
#include <cmath>


namespace carto {
    double LatLongUtils::microdegreesToDegrees(int coordinate) {
        double value = coordinate / MFConstants::_CONVERSION_FACTOR;
        return value;
    }

    MapBounds LatLongUtils::enlarge(const MapBounds& bounds, int meters) {
        if (meters < 0) {
            throw GenericException("MapBounds::enlarge: BoundingBox extend operation does not accept negative values");
        }

        double verticalExpansion = latitudeDistance(meters);
        double horizontalExpansion = longitudeDistance(meters, std::max(std::abs(bounds.getMin().getY()), std::abs(bounds.getMax().getY())));

        double minLat = std::max(MFConstants::_LATITUDE_MIN, bounds.getMin().getY() - verticalExpansion);
        double minLon = std::max(-180.0, bounds.getMin().getX() - horizontalExpansion);
        double maxLat = std::min(MFConstants::_LATITUDE_MAX, bounds.getMax().getY() + verticalExpansion);
        double maxLon = std::min(180.0, bounds.getMax().getX() + horizontalExpansion);

        MapPos min(minLon, minLat);
        MapPos max(maxLon, maxLat);
        return { min, max };
    }

    bool LatLongUtils::intersectsArea(const MapBounds &mapBounds, const std::vector<std::vector<MapPos>> &wayBlocks) {

        // x is latitude, y is longitude

        if (wayBlocks.empty() || wayBlocks[0].empty()) {
            return false;
        }

        for (auto const &coordinateBlock : wayBlocks) {
            for (auto const &coordinate : coordinateBlock) {
                if (mapBounds.contains(coordinate)) {
                    // if any of the points is inside the map bounds return early
                    return true;
                }
            }
        }

        // no fast exit
        double tmpMinLat = wayBlocks[0][0].getY();
        double tmpMinLon = wayBlocks[0][0].getX();
        double tmpMaxLat = wayBlocks[0][0].getY();
        double tmpMaxLon = wayBlocks[0][0].getX();

        // calculate the maximum extent
        for (auto const &outer : wayBlocks) {
            for (auto const &coord : outer) {
                tmpMinLat = std::min(tmpMinLat, coord.getY());
                tmpMaxLat = std::max(tmpMaxLat, coord.getY());
                tmpMinLon = std::min(tmpMinLon, coord.getX());
                tmpMaxLon = std::max(tmpMaxLon, coord.getX());
            }
        }

        MapPos min(tmpMinLon, tmpMinLat);
        MapPos max(tmpMaxLon, tmpMaxLat);
        MapBounds checkBox(min, max);

        // perform the intersection check
        return mapBounds.intersects(checkBox);
    }

    double LatLongUtils::latitudeDistance(uint32_t meters) {
        return (meters * 360) / (2 * Const::PI * Const::EARTH_RADIUS);
    }

    double LatLongUtils::longitudeDistance(uint32_t meters, double lat) {
        return (meters * 360) / (2 * Const::PI * Const::EARTH_RADIUS * std::cos((lat * Const::PI) / 180));
    }
}
