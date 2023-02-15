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

#include "mapsforge-reader/QueryParameters.h"
#include "mapsforge-reader/QueryCalculations.h"

namespace carto {
    QueryParameters::QueryParameters() {}

    void QueryParameters::calculateTiles(const MapTile &tile, const SubFileParameters &subFileParameters) {
        if (tile.zoom < subFileParameters.getBaseZoomLevel()) {
            // calculate the XY numbers of the upper left and lower right sub-tiles
            int zoomLevelDiff = subFileParameters.getBaseZoomLevel() - tile.zoom;
            _from_base_tile_x = tile.x << zoomLevelDiff;
            _from_base_tile_y = tile.y << zoomLevelDiff;
            _to_base_tile_x = _from_base_tile_x + (1 << zoomLevelDiff) - 1;
            _to_base_tile_y = _from_base_tile_y + (1 << zoomLevelDiff) - 1;
            _use_tile_bitmask = false;
        } else if (tile.zoom > subFileParameters.getBaseZoomLevel()) {
            // calculate the XY numbers of the parent base tile
            int zoomLevelDiff = tile.zoom - subFileParameters.getBaseZoomLevel();
            _from_base_tile_x = (uint32_t)tile.x >> zoomLevelDiff;
            _from_base_tile_y = (uint32_t)tile.y >> zoomLevelDiff;
            _to_base_tile_x = _from_base_tile_x;
            _to_base_tile_y = _from_base_tile_y;
            _use_tile_bitmask = true;
            _query_tile_bitmask = QueryCalculations::calculateTileBitmask(tile, zoomLevelDiff);
        } else {
            _from_base_tile_x = tile.x;
            _from_base_tile_y = tile.y;
            _to_base_tile_x = _from_base_tile_x;
            _to_base_tile_y = _from_base_tile_y;
            _use_tile_bitmask = false;
        }
    }

    void QueryParameters::calculateBlocks(const SubFileParameters &subFileParameters) {
        // calculate the blocks in the file which need to be read
        _from_block_x = std::max((int64_t)(_from_base_tile_x - subFileParameters.getBoundaryTileLeft()), (int64_t)0);
        _from_block_y = std::max((int64_t)(_from_base_tile_y - subFileParameters.getBoundaryTileTop()), (int64_t)0);
        _to_block_x = std::min((int64_t)(_to_base_tile_x - subFileParameters.getBoundaryTileLeft()), (int64_t)(subFileParameters.getBlocksWidth() - 1));
        _to_block_y = std::min((int64_t)(_to_base_tile_y - subFileParameters.getBoundaryTileTop()), (int64_t)(subFileParameters.getBlocksHeight() - 1));
    }

    void QueryParameters::setQueryZoomLevel(const int &queryZoomLevel) {
        _query_tile_zoom_level = queryZoomLevel;
    }

    const int64_t & QueryParameters::getFromBaseTileX() const {
        return _from_base_tile_x;
    }

    const int64_t & QueryParameters::getFromBaseTileY() const {
        return _from_base_tile_y;
    }

    const int64_t & QueryParameters::getFromBlockX() const {
        return _from_block_x;
    }

    const int64_t & QueryParameters::getFromBlockY() const {
        return _from_block_y;
    }

    const int32_t & QueryParameters::getQueryZoomLevel() const {
        return _query_tile_zoom_level;
    }

    const uint16_t & QueryParameters::getQueryTileBitmask() const {
        return _query_tile_bitmask;
    }

    const int64_t & QueryParameters::getToBaseTileX() const {
        return _to_base_tile_x;
    }

    const int64_t & QueryParameters::getToBaseTileY() const {
        return _to_base_tile_y;
    }

    const int64_t & QueryParameters::getToBlockX() const {
        return _to_block_x;
    }

    const int64_t & QueryParameters::getToBlockY() const {
        return _to_block_y;
    }

    const bool & QueryParameters::getUseTileBitmask() const {
        return _use_tile_bitmask;
    }

    bool QueryParameters::operator==(const QueryParameters &other) const {
        return  _from_base_tile_y == other.getFromBaseTileY() &&
                _from_base_tile_x == other.getFromBaseTileX() &&
                _from_block_y == other.getFromBlockY() &&
                _from_block_x == other.getFromBlockX() &&
                _to_base_tile_y == other.getToBaseTileY() &&
                _to_base_tile_x == other.getToBaseTileX() &&
                _to_block_y == other.getToBlockY() &&
                _to_block_x == other.getToBlockX() &&
                _use_tile_bitmask == other._use_tile_bitmask &&
                _query_tile_bitmask == other.getQueryTileBitmask() &&
                _query_tile_zoom_level == other.getQueryZoomLevel();
    }

    bool QueryParameters::operator!=(const QueryParameters &other) const {
        return !(*this == other);
    }
}
