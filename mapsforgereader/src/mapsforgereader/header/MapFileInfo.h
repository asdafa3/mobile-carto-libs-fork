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

#ifndef CARTO_MOBILE_SDK_MAPFILEINFO_H
#define CARTO_MOBILE_SDK_MAPFILEINFO_H


#include <mapsforgereader/model/Tag.h>
#include <mapsforgereader/header/MapFileInfoBuilder.h>

#include <string>

namespace carto {

    /**
     * Container class that stores all metadata contained in the header
     * of a .map file. This includes optional and required fields. Sub file
     * configurations are not contained.
     */
    class MapFileInfo {
    public:
        typedef cglib::vec3<double> MapPos;
        typedef cglib::bbox3<double> MapBounds;
        typedef vt::TileId MapTile;

        MapFileInfo(const MapFileInfoBuilder &map_file_info_builder);

        ~MapFileInfo();

        const MapBounds &getBoundingBox() const;

        const uint32_t &getFileVersion() const;

        const std::string &getComment() const;

        const std::string &getCreatedBy() const;

        const bool &getDebugInfo() const;

        const uint64_t &getFileSize() const;

        const std::string &getLanguagePreference() const;

        const uint64_t &getMapDate() const;

        const uint8_t &getNumberOfSubFiles() const;

        const std::vector<Tag> &getPoiTags() const;

        const std::vector<Tag> &getWayTags() const;

        const std::string &getProjectionName() const;

        const MapPos &getStartPosition() const;

        const uint8_t &getStartZoomLevel() const;

        const uint32_t &getTilePixelSize() const;

        const uint8_t &getMinZoomLevel() const;

        const uint8_t &getMaxZoomLevel() const;

        std::string toString() const;
    private:

        /**
         * EPSG:3857 projected bounding box spanned by min and max position.
         */
        MapBounds _bounding_box;

        /**
         * The version number of the file.
         */
        uint32_t _file_version;

        /**
         * An optional comment by the author.
         */
        std::string _comment;

        /**
         * The author of this map file.
         */
        std::string _created_by;

        /**
         * Indicator for existence of debug information.
         */
        bool _debug_file;

        /**
         * Total file size of a .map file.
         */
        uint64_t _file_size;

        /**
         * Language preference string (en, de ...).
         */
        std::string _lang_preference;

        /**
         * Date of creation.
         */
        uint64_t _map_date;

        /**
         * Number of zoom intervals / sub files in this map file.
         */
        uint8_t _number_of_sub_files;

        /**
         * All POIs tags used in the map file.
         */
        std::vector<Tag> _poi_tags;

        /**
         * All way tags in this map file.
         */
        std::vector<Tag> _way_tags;

        /**
         * The name of the projection used for creating this map file.
         */
        std::string _projection_name;

        /**
         * An optional start position in lat / lon coordinates.
         */
        MapPos _start_position;

        /**
         * Optional start zoom level.
         */
        uint8_t _start_zoom_level;

        /**
         * Size of a map tile in pixels.
         */
        uint32_t _tile_pixel_size;

        /**
         * The min zoom level supported by this file.
         */
        uint8_t _zoom_level_min;

        /**
         * The max zoom level supported by this file.
         */
        uint8_t _zoom_level_max;
    };
}


#endif //CARTO_MOBILE_SDK_MAPFILEINFO_H
