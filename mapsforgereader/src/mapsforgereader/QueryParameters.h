/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014-2015 Ludwig M Brinckmann
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

#ifndef CARTO_MOBILE_SDK_QUERYPARAMETERS_H
#define CARTO_MOBILE_SDK_QUERYPARAMETERS_H

#include <mapsforgereader/header/SubFileParameters.h>
#include <mapnikvt/Types.h>

namespace carto {
    /**
     * Calculates base tiles covered by a query tile on the base zoom level of a sub file.
     */
    class QueryParameters {
    public:

        
        
        QueryParameters();

        /**
         * Calculates tiles covered by a query tile on the base zoom level of a sub file.
         * Dependant on the difference of the base and query zoom level, either all sub tiles
         * of the query tile are calculated (neg. diff) or the parent tile on the base
         * zoom level is calculated (pos. diff) and a bitmask is calculated to filter only the relevant
         * sub tiles. If the query tile is on the base zoom level (zero diff), the tile coordinates
         * are used. This is needed as data is only stored for the base zoom level.
         *
         * @param tile Tile XYZ coordinates.
         * @param subFileParameters Zoom interval configuration the query tile belongs to.
         */
        void calculateTiles(const mvt::MapTile &tile, const SubFileParameters &subFileParameters);

        /**
         * Calculates blocks as used by Mapsforge from regular tile coordiantes.
         * Blocks resemble tiles, but start from (0,0) and can be uniquely identified by one
         * number.
         *
         * @param subFileParameters Zoom interval configuration the query tile belongs to.
         */
        void calculateBlocks(const SubFileParameters &subFileParameters);

        /**
         * Sets the query zoom level used for base tile calculation.
         * @param queryZoomLevel
         */
        void setQueryZoomLevel(const int &queryZoomLevel);

        /**
         * Get the query zoom level.
         * @return Zoom level of query tile.
         */
        const int32_t & getQueryZoomLevel() const;

        /**
         * Get the leftmost tile x coordinate.
         * @return Leftmost tile x coordinate (int).
         */
        const int64_t & getFromBaseTileX() const;

        /**
         * Get the topmost tile y coordinate.
         * @return Topmost tile y coordinate (int).
         */
        const int64_t & getFromBaseTileY() const;

        /**
         * Get the leftmost block x coordinate.
         * @return Leftmost block x coordinate (int).
         */
        const int64_t & getFromBlockX() const;

        /**
         * Get the topmost block y coordinate.
         * @return Topmost block y coordinate (int).
         */
        const int64_t & getFromBlockY() const;

        /**
         * Returns the calculated query tile bitmask.
         * Bitmask is calculated during base tile calculation.
         * @return Bitmask for query tile.
         */
        const uint16_t & getQueryTileBitmask() const;

        /**
         * Get the rightmost tile y coordinate.
         * @return Rightmost tile y coordinate (int).
         */
        const int64_t & getToBaseTileX() const;

        /**
         * Get the bottommost tile y coordinate.
         * @return Bottommost tile y coordinate (int).
         */
        const int64_t & getToBaseTileY() const;

        /**
         * Get the rightmost block y coordinate.
         * @return Rightmost block y coordinate (int).
         */
        const int64_t & getToBlockX() const;

        /**
         * Get the tile bottommost block y coordinate.
         * @return Bottommost block y coordinate (int).
         */
        const int64_t & getToBlockY() const;

        /**
         * Indicates if a bitmask is used for the tile.
         *
         * @return True for query tiles higher than the base zoom level.
         */
        const bool & getUseTileBitmask() const;

        bool operator ==(const QueryParameters &other) const;

        bool operator !=(const QueryParameters &other) const;
    private:
        /**
         * Leftmost base tile coordinate.
         */
        int64_t _from_base_tile_x;

        /**
         * Topmost base tile coordinate.
         */
        int64_t _from_base_tile_y;

        /**
         * Leftmost block coordinate.
         */
        int64_t _from_block_x;

        /**
         * Topmost block coordinate.
         */
        int64_t _from_block_y;

        /**
         * Optional bitmask for sub tiles.
         */
        uint16_t _query_tile_bitmask;

        /**
         * The zoom level of the query tile.
         */
        int32_t _query_tile_zoom_level;

        /**
         * The rightmost tile X coordinate.
         */
        int64_t _to_base_tile_x;

        /**
         * The bottommost tile Y coordinate.
         */
        int64_t _to_base_tile_y;

        /**
         * The rightmost block X coordinate.
         */
        int64_t _to_block_x;

        /**
         * The bottommost block y coordinate.
         */
        int64_t _to_block_y;

        /**
         * Indicates existence of a tile bitmask for the current tile.
         */
        bool _use_tile_bitmask;
    };
}


#endif //CARTO_MOBILE_SDK_QUERYPARAMETERS_H
