/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014 Ludwig M Brinckmann
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

#ifndef CARTO_MOBILE_SDK_QUERYCALCULATIONS_H
#define CARTO_MOBILE_SDK_QUERYCALCULATIONS_H

#include <mapnikvt/Types.h>

namespace carto {

    /**
     * Utility class for calculating tile bitmasks for query tiles dependant on their zoom level.
     */
    class QueryCalculations {
    public:

        using MapTile = carto::mvt::MapTile;

        /**
         * Calculate tile bitmask for tiles inside a zoom interval that are
         * on a higher zoom level than the base zoom level. Based on the difference
         * two the base zoom level, two cases are differenciated.
         *
         * A bitmask is stored as a 16 Bit integer. Each bit mask one second
         * level sub tile on base zoom level + 2. (1 tile on zoom level z <=> 16 tiles on zoom level z+2)
         *
         * E.g. the following short integer would allow only the upper left quadrant.
         * 0xcc00
         * =>
         * 1 1 0 0 | 1 1 0 0 | 0 0 0 0 | 0 0 0 0
         * =>
         * 1 1 0 0
         * 1 1 0 0
         * 0 0 0 0
         * 0 0 0 0
         *
         * Case A: Diff = 1 => Calculate first level sub tile bitmask.
         * Case B: Diff > 1 => Map tile to second level sub tile relative to base zoom level and calculate a second level bit mask.
         *
         * @param tile Tile definition in XYZ scheme.
         * @param zoomLevelDifference Difference of tile zoom level to base zoom level.
         * @return First or second level subtile bitmask.
         */
        static uint16_t calculateTileBitmask(const MapTile &tile, int zoomLevelDifference);
    private:

        /**
         * Calculate first level tile bitmask (diff == 1).
         *
         * @param tile Tile definition in XYZ scheme.
         * @return First level sub tile bitmask.
         */
        static uint16_t getFirstLevelTileBitmask(const MapTile &tile);

        /**
         * Calculate second level bitmask for the lower left quadrant.
         *
         * @param subTileX X number of the second level sub tile.
         * @param subtileY Y number of the second level sub tile.
         * @return 16 Bit Integer Bitmask.
         */
        static uint16_t getSecondLevelTileBitmaskLowerLeft(uint32_t subTileX, uint32_t subtileY);

        /**
         * Calculate second level bitmask for the lower right quadrant.
         *
         * @param subTileX X number of the second level sub tile.
         * @param subtileY Y number of the second level sub tile.
         * @return 16 Bit Integer Bitmask.
         */
        static uint16_t getSecondLevelTileBitmaskLowerRight(uint32_t subTileX, uint32_t subtileY);

        /**
         * Calculate second level bitmask for the upper left quadrant.
         *
         * @param subTileX X number of the second level sub tile.
         * @param subtileY Y number of the second level sub tile.
         * @return 16 Bit Integer Bitmask.
         */
        static uint16_t getSecondLevelTileBitmaskUpperLeft(uint32_t subTileX, uint32_t subtileY);

        /**
         * Calculate second level bitmask for the upper right quadrant.
         *
         * @param subTileX X number of the second level sub tile.
         * @param subtileY Y number of the second level sub tile.
         * @return 16 Bit Integer Bitmask.
         */
        static uint16_t getSecondLevelTileBitmaskUpperRight(uint32_t subTileX, uint32_t subtileY);

        QueryCalculations();
    };
}


#endif //CARTO_MOBILE_SDK_QUERYCALCULATIONS_H
