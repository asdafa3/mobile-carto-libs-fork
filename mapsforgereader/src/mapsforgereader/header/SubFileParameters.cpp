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

#include "SubFileParameters.h"
#include "projections/EPSG3857.h"
#include "projections/EPSG4326.h"
// #include "utils/TileUtils.h"
#include <mapsforgereader/utils/TileUtils.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <mapsforgereader/MFConstants.h>

namespace carto {
    SubFileParameters::SubFileParameters() { }

    SubFileParameters::SubFileParameters(const SubFileParameterBuilder &subFileParameterBuilder) {
        _start_address = subFileParameterBuilder._start_address;
        _index_start_address = subFileParameterBuilder._index_start_address;
        _sub_file_size = subFileParameterBuilder._sub_file_size;
        _base_zoom_level = subFileParameterBuilder._base_zoom_level;
        _zoom_level_min = subFileParameterBuilder._zoom_level_min;
        _zoom_level_max = subFileParameterBuilder._zoom_level_max;

        // use pseudo mercator projection
        std::shared_ptr<EPSG3857> proj = std::make_shared<EPSG3857>();

        // calculate positions that span the bbox of the map extent
        MapPos projectedTopLeft = subFileParameterBuilder._bounding_box.min;
        MapPos projectedBottomRight = subFileParameterBuilder._bounding_box.max;

        // calculate tiles that span the bbox of the map extent
        MapTile topLeftTile = TileUtils::CalculateMapTile(projectedTopLeft, _base_zoom_level).getFlipped();
        MapTile bottomRightTile = TileUtils::CalculateMapTile(projectedBottomRight, _base_zoom_level).getFlipped();

        // top and bottom are switched
        _boundary_tile_bottom = topLeftTile.y;
        _boundary_tile_top = bottomRightTile.y;
        _boundary_tile_left = topLeftTile.x;
        _boundary_tile_right = bottomRightTile.x;

        // num blocks as difference between tile bounds in each spatial direction
        _blocks_width = _boundary_tile_right - _boundary_tile_left + 1;
        _blocks_height = _boundary_tile_bottom - _boundary_tile_top + 1;
        _number_of_blocks = _blocks_width * _blocks_height;
        _index_end_address = _index_start_address + _number_of_blocks * MFConstants::_BYTES_PER_INDEX_ENTRY;
    }

    SubFileParameters::~SubFileParameters() { }

    const uint8_t &SubFileParameters::getBaseZoomLevel() const {
        return _base_zoom_level;
    }

    const uint64_t &SubFileParameters::getBlocksHeight() const {
        return _blocks_height;
    }

    const uint64_t &SubFileParameters::getBlocksWidth() const {
        return _blocks_width;
    }

    const uint64_t &SubFileParameters::getBoundaryTileBottom() const {
        return _boundary_tile_bottom;
    }

    const uint64_t &SubFileParameters::getBoundaryTileTop() const {
        return _boundary_tile_top;
    }

    const uint64_t &SubFileParameters::getBoundaryTileLeft() const {
        return _boundary_tile_left;
    }

    const uint64_t &SubFileParameters::getBoundaryTileRight() const {
        return _boundary_tile_right;
    }

    const uint64_t &SubFileParameters::getIndexEndAddress() const {
        return _index_end_address;
    }

    const uint64_t &SubFileParameters::getIndexStartAddress() const {
        return _index_start_address;
    }

    const uint64_t &SubFileParameters::getNumberOfBlocks() const {
        return _number_of_blocks;
    }

    const uint64_t &SubFileParameters::getStartAddress() const {
        return _start_address;
    }

    const uint8_t &SubFileParameters::getMinZoomLevel() const {
        return _zoom_level_min;
    }

    const uint8_t &SubFileParameters::getMaxZoomLevel() const {
        return _zoom_level_max;
    }

    const uint64_t &SubFileParameters::getSubFileSize() const {
        return _sub_file_size;
    }

    std::string SubFileParameters::toString() const {
        std::stringstream ss;
        ss << std::setiosflags(std::ios::fixed);
        ss << "base zoom: " << _base_zoom_level << "\n";
        ss << "block height: " << _blocks_height << "\n";
        ss << "block width: " << _blocks_width << "\n";
        ss << "boundary tile left (x left): " << _boundary_tile_left << "\n";
        ss << "boundary tile right (x right): " << _boundary_tile_right << "\n";
        ss << "boundary tile top (y top): " << _boundary_tile_top << "\n";
        ss << "boundary tile bottom (y bottom): " << _boundary_tile_bottom << "\n";
        ss << "index start address: " << _index_start_address << "\n";
        ss << "index end address: " << _index_end_address << "\n";
        ss << "number of blocks: " << _number_of_blocks << "\n";
        ss << "start address: " << _start_address << "\n";
        ss << "min zoom level: " << _zoom_level_min << "\n";
        ss << "max zoom level: " << _zoom_level_max << "\n";
        ss << "sub file size: " << _sub_file_size << "\n";
        return ss.str();
    }
}
