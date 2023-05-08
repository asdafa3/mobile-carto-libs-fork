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


#ifndef CARTO_MOBILE_SDK_LATLONGUTILS_H
#define CARTO_MOBILE_SDK_LATLONGUTILS_H

#include <cglib/bbox.h>
#include <cglib/vec.h>
#include <vector>

namespace carto {

    /**
     * Utility class to perform operations on latitudes and longitudes.
     */
    class LatLongUtils {
    public: 
        typedef cglib::vec2<double> MapPos;
        typedef cglib::bbox2<double> MapBounds;

        /**
         * Converts lat lon in microdegrees to degrees.
         *
         * @param coordinate Microdegree coordinate.
         * @return
         */
        static double microdegreesToDegrees(int coordinate);

        /**
         *
         * @param mapBounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
         * @param wayBlocks List of subsequent way blocks from a .map file.
         * @return True of part of the way block intersect with the bounding box.
         */
        static bool intersectsArea(const MapBounds &mapBounds, const std::vector<std::vector<MapPos>> &wayBlocks);

        /**
         * Calculates the vertical latitude extent from meters.
         *
         * @param meters Meters in vertical direction.
         * @return Degrees of distance in meters.
         */
        static double latitudeDistance(uint32_t meters);

        /**
         * Calculates the horizontal longitude extent from meters.
         *
         * @param meters Meters in horizontal direction.
         * @return Degrees of distance in meters.
         */
        static double longitudeDistance(uint32_t meters, double lat);

        /**
         * Enlarges a bounding box by some meters.
         *
         * @param bounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
         * @param meters Meters in each spatial direction.
         * @return Enlarged bounding box.
         */
        static MapBounds enlarge(const MapBounds& bounds, int meters);
    private:
        LatLongUtils();
    };
}


#endif //CARTO_MOBILE_SDK_LATLONGUTILS_H
