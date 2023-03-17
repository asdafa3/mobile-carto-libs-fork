/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014 Ludwig M Brinckmann
 * Copyright 2016 devemux86
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

#include "MapFileInfo.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace carto {
    MapFileInfo::MapFileInfo(const MapFileInfoBuilder &map_file_info_builder) {
        _comment = map_file_info_builder._optionalFields -> getComment();
        _created_by = map_file_info_builder._optionalFields -> getCreatedBy();
        _debug_file = map_file_info_builder._optionalFields -> getIsDebugFile();
        _file_size = map_file_info_builder._file_size;
        _file_version = map_file_info_builder._file_version;
        _lang_preference = map_file_info_builder._optionalFields -> getLanguagePreference();
        _bounding_box = map_file_info_builder._bounding_box;
        _map_date = map_file_info_builder._map_date;
        _number_of_sub_files = map_file_info_builder._number_of_sub_files;
        _start_zoom_level = map_file_info_builder._optionalFields -> getStartZoomLevel();
        _start_position = map_file_info_builder._optionalFields -> getStartPosition();
        _tile_pixel_size = map_file_info_builder._tile_pixel_size;
        _zoom_level_max = map_file_info_builder._zoom_level_max;
        _zoom_level_min = map_file_info_builder._zoom_level_min;
        _poi_tags = map_file_info_builder._poi_tags;
        _way_tags = map_file_info_builder._way_tags;
        _projection_name = map_file_info_builder._projection_name;
    }

    MapFileInfo::~MapFileInfo() {}

    // Getters and setters

    const MapBounds &MapFileInfo::getBoundingBox() const {
        return _bounding_box;
    }

    const uint32_t &MapFileInfo::getFileVersion() const {
        return _file_version;
    }

    const std::string &MapFileInfo::getComment() const {
        return _comment;
    }

    const std::string &MapFileInfo::getCreatedBy() const {
        return _created_by;
    }

    const bool &MapFileInfo::getDebugInfo() const {
        return _debug_file;
    }

    const uint64_t &MapFileInfo::getFileSize() const {
        return _file_size;
    }

    const std::string &MapFileInfo::getLanguagePreference() const {
        return _lang_preference;
    }

    const uint64_t &MapFileInfo::getMapDate() const {
        return _map_date;
    }

    const uint8_t &MapFileInfo::getNumberOfSubFiles() const {
        return _number_of_sub_files;
    }

    const std::vector<Tag> &MapFileInfo::getPoiTags() const {
        return _poi_tags;
    }

    const std::vector<Tag> &MapFileInfo::getWayTags() const {
        return _way_tags;
    }

    const std::string &MapFileInfo::getProjectionName() const {
        return _projection_name;
    }

    const MapPos &MapFileInfo::getStartPosition() const {
        return _start_position;
    }

    const uint8_t &MapFileInfo::getStartZoomLevel() const {
        return _start_zoom_level;
    }

    const uint32_t &MapFileInfo::getTilePixelSize() const {
        return _tile_pixel_size;
    }

    const uint8_t &MapFileInfo::getMinZoomLevel() const {
        return _zoom_level_min;
    }

    const uint8_t &MapFileInfo::getMaxZoomLevel() const {
        return _zoom_level_max;
    }

    // debugging

    std::string MapFileInfo::toString() const {
        std::stringstream ss;
        ss << std::setiosflags(std::ios::fixed);
        ss << "bounding box: " << _bounding_box.toString() << "\n";
        ss << "comment: " << _comment << "\n";
        ss << "created by: " << _created_by << "\n";
        ss << "debug info: " << _debug_file << "\n";
        ss << "file size: " << _file_size << "\n";
        ss << "language preference: " << _lang_preference << "\n";
        ss << "creation date: " << _map_date << "\n";
        ss << "number of sub files: " << _number_of_sub_files << "\n";
        ss << "number of poi tags: " << _poi_tags.size() << "\n";
        ss << "number of way tags: " << _way_tags.size() << "\n";
        ss << "projection name: " << _projection_name << "\n";
        ss << "start position: " << _start_position.toString() << "\n";
        ss << "start zoom level: " << _start_zoom_level << "\n";
        ss << "min zoom level: " << _zoom_level_min << "\n";
        ss << "max zoom level: " << _zoom_level_max << "\n";
        ss << "tile pixel size: " << _tile_pixel_size << "\n";
        return ss.str();
    }
}