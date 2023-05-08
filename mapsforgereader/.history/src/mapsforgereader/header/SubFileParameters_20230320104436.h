/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
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

#ifndef CARTO_MOBILE_SDK_SUBFILEPARAMETERS_H
#define CARTO_MOBILE_SDK_SUBFILEPARAMETERS_H

#include <mapsforgereader/header/SubFileParameterBuilder.h>
#include <cglib/vec.h>
#include <vt/TileId.h>

namespace carto {
    /**
     * Stores zoom interval information for the sub files stored in each
     * .map file.
     */
    class SubFileParameters {
    public:
        typedef vt::TileId MapTile;
        typedef cglib::vec2<double> MapPos;

        SubFileParameters();
        SubFileParameters(const SubFileParameterBuilder &subFileParameterBuilder);
        ~SubFileParameters();

        const uint8_t &getBaseZoomLevel() const;

        const uint64_t &getBlocksHeight() const;

        const uint64_t &getBlocksWidth() const;

        const uint64_t &getBoundaryTileBottom() const;

        const uint64_t &getBoundaryTileTop() const;

        const uint64_t &getBoundaryTileLeft() const;

        const uint64_t &getBoundaryTileRight() const;

        const uint64_t &getIndexEndAddress() const;

        const uint64_t &getIndexStartAddress() const;

        const uint64_t &getNumberOfBlocks() const;

        const uint64_t &getStartAddress() const;

        const uint8_t &getMinZoomLevel() const;

        const uint8_t &getMaxZoomLevel() const;

        const uint64_t &getSubFileSize() const;

        /**
         * Constructs a string of sub file config values for printing and debugging.
         *
         * @return Debug string representation of sub file.
         */
        std::string toString() const;
    private:
        /**
         * The base zoom level of a sub file. This is the level where
         * Mapsforge blocks are stored.
         */
        uint8_t _base_zoom_level;

        /**
         * Number of blocks in negative Y direction (top down).
         */
        uint64_t _blocks_height;

        /**
         * Number of blocks in positive x direction.
         */
        uint64_t _blocks_width;

        /**
         * Y coordinate of bottommost tile.
         */
        uint64_t _boundary_tile_bottom;

        /**
         * Y coordinate of topmost tile.
         */
        uint64_t _boundary_tile_top;

        /**
         * X coordinate of leftmost tile.
         */
        uint64_t _boundary_tile_left;

        /**
         * X coordinate of rightmost tile.
         */
        uint64_t _boundary_tile_right;

        /**
         * (Real) start position of index block segment in sub file in .map file.
         */
        uint64_t _index_end_address;

        /**
         * End position of index block segment in sub file.
         */
        uint64_t _index_start_address;

        /**
         * Total number of blocks in bounds.
         */
        uint64_t _number_of_blocks;

        /**
         * Start position of index block segment in sub file in .map file. Not
         * always the start of the sub files index segment as debug information can be
         * stored beforehand.
         */
        uint64_t _start_address;

        /**
         * The maximum supported zoom level of this sub file.
         */
        uint8_t _zoom_level_max;

        /**
         * The minimum supported zoom level of this sub file.
         */
        uint8_t _zoom_level_min;

        /**
         * The size of a sub file (size of index and data segment combined).
         */
        uint64_t _sub_file_size;
    };
}


#endif //CARTO_MOBILE_SDK_SUBFILEPARAMETERS_H
