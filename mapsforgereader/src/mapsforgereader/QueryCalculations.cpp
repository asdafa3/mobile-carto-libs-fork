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

#include "QueryCalculations.h"

namespace carto {

    QueryCalculations::QueryCalculations() { }

    uint16_t QueryCalculations::calculateTileBitmask(const MapTile &tile, int zoomLevelDifference) {
        if (zoomLevelDifference == 1) {
            return getFirstLevelTileBitmask(tile);
        }

        // calculate the XY numbers of the second level sub-tile
        uint64_t subTileX = (uint32_t)tile.x >> (zoomLevelDifference - 2);
        uint64_t subTileY = (uint32_t)tile.y >> (zoomLevelDifference - 2);

        // calculate the XY numbers of the parent tile
        uint64_t parentTileX = subTileX >> 1;
        uint64_t parentTileY = subTileY >> 1;

        // determine bitmask for all 16 sub tiles
        if (parentTileX % 2 == 0 && parentTileY % 2 == 0) {
            return getSecondLevelTileBitmaskUpperLeft(subTileX, subTileY);
        } else if (parentTileX % 2 == 1 && parentTileY % 2 == 0) {
            return getSecondLevelTileBitmaskUpperRight(subTileX, subTileY);
        } else if (parentTileX % 2 == 0 && parentTileY % 2 == 1) {
            return getSecondLevelTileBitmaskLowerLeft(subTileX, subTileY);
        } else {
            return getSecondLevelTileBitmaskLowerRight(subTileX, subTileY);
        }
    }

    uint16_t QueryCalculations::getFirstLevelTileBitmask(const MapTile &tile) {
        if (tile.x % 2 == 0 && tile.y % 2 == 0) {
            // upper left quadrant
            return 0xcc00;
        } else if (tile.x % 2 == 1 && tile.y % 2 == 0) {
            // upper right quadrant
            return 0x3300;
        } else if (tile.x % 2 == 0 && tile.y % 2 == 1) {
            // lower left quadrant
            return 0xcc;
        } else {
            // lower right quadrant
            return 0x33;
        }
    }

    uint16_t QueryCalculations::getSecondLevelTileBitmaskLowerLeft(uint32_t subTileX, uint32_t subtileY) {
        if (subTileX % 2 == 0 && subtileY % 2 == 0) {
            // upper left sub tile
            return 0x80;
        } else if (subTileX % 2 == 1 && subtileY % 2 == 0) {
            // upper right sub tile
            return 0x40;
        } else if (subTileX % 2 == 0 && subtileY % 2 == 1) {
            // lower left sub tile
            return 0x8;
        } else {
            // lower right sub tile
            return 0x4;
        }
    }

    uint16_t QueryCalculations::getSecondLevelTileBitmaskLowerRight(uint32_t subTileX, uint32_t subtileY) {
        if (subTileX % 2 == 0 && subtileY % 2 == 0) {
            // upper left sub tile
            return 0x20;
        } else if (subTileX % 2 == 1 && subtileY % 2 == 0) {
            // upper right sub tile
            return 0x10;
        } else if (subTileX % 2 == 0 && subtileY % 2 == 1) {
            // lower left sub tile
            return 0x2;
        } else {
            // lower right sub tile
            return 0x1;
        }
    }

    uint16_t QueryCalculations::getSecondLevelTileBitmaskUpperLeft(uint32_t subTileX, uint32_t subtileY) {
        if (subTileX % 2 == 0 && subtileY % 2 == 0) {
            // upper left sub tile
            return 0x8000;
        } else if (subTileX % 2 == 1 && subtileY % 2 == 0) {
            // upper right sub tile
            return 0x4000;
        } else if (subTileX % 2 == 0 && subtileY % 2 == 1) {
            // lower left sub tile
            return 0x800;
        } else {
            // lower right sub tile
            return 0x400;
        }
    }

    uint16_t QueryCalculations::getSecondLevelTileBitmaskUpperRight(uint32_t subTileX, uint32_t subtileY) {
        if (subTileX % 2 == 0 && subtileY % 2 == 0) {
            // upper left sub tile
            return 0x2000;
        } else if (subTileX % 2 == 1 && subtileY % 2 == 0) {
            // upper right sub tile
            return 0x1000;
        } else if (subTileX % 2 == 0 && subtileY % 2 == 1) {
            // lower left sub tile
            return 0x200;
        } else {
            // lower right sub tile
            return 0x100;
        }
    }
}
