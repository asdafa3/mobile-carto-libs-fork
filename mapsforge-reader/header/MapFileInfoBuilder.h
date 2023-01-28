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

#ifndef CARTO_MOBILE_SDK_MAPFILEINFOBUILDER_H
#define CARTO_MOBILE_SDK_MAPFILEINFOBUILDER_H

#include "core/MapBounds.h"
#include "core/MapPos.h"
#include "mapsforge-reader/model/Tag.h"
#include "mapsforge-reader/header/OptionalFields.h"
#include <string>

namespace carto {

    class MapFileInfo;

    /**
     * Builds MapFileInfo objects from optional and required fields
     * of the .map file header segment.
     */
    class MapFileInfoBuilder {
    public:
        MapFileInfoBuilder();

        MapFileInfo build() const;

        MapBounds _bounding_box;
        uint64_t _file_size;
        uint32_t _file_version;
        uint64_t _map_date;
        uint8_t _number_of_sub_files;
        std::unique_ptr<OptionalFields> _optionalFields;
        std::vector<Tag> _poi_tags;
        std::vector<Tag> _way_tags;
        std::string _projection_name;
        uint32_t _tile_pixel_size;
        uint8_t _zoom_level_min;
        uint8_t _zoom_level_max;
    };
}


#endif //CARTO_MOBILE_SDK_MAPFILEINFOBUILDER_H
