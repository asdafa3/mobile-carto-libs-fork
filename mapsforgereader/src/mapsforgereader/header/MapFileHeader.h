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

#ifndef CARTO_MOBILE_SDK_MAPFILEHEADER_H
#define CARTO_MOBILE_SDK_MAPFILEHEADER_H

#include <mapsforgereader/ReadBuffer.h>
#include "SubFileParameters.h"
#include <mapsforgereader/header/MapFileInfo.h>
#include <climits>
#include "mapnikvt/Logger.h"

#include <memory>
#include <tinyformat.h>

namespace carto {
    /**
     * Container for all data contained in the meta data / header segment of
     * a .map file. This includes casual metadata, map information, tags storage for pois
     * and ways and description of the sub files (zoom intervals) this .map file is composed of.
     */
    class MapFileHeader {
    public:
        using Logger = carto::mvt::Logger;

        MapFileHeader(std::shared_ptr<mvt::Logger> logger);
        ~MapFileHeader();

        uint8_t getQueryZoomLevel(uint8_t zoom_level);

        SubFileParameters getSubFileParameters(int query_zoom_level);

        /**
         * Read and parse all data that is contained in the map file header.
         * Decomposed into metadata fields and zoom interval configuration.
         *
         * @param read_buffer The buffer containing the header data.
         * @param file_size The size of the .map file.
         */
        void readHeader(const std::shared_ptr<ReadBuffer> &read_buffer, uint64_t file_size);

        const std::shared_ptr<MapFileInfo> & getMapFileInfo() const;
    private:

        /**
         * Read zoom interval configuration stored at the end of the header segment.
         * @param read_buffer The buffer containing the header data.
         * @param file_size The size of the .map file.
         * @param mapFileInfoBuilder Builder object to build map file info from header meta data.
         */
        void readSubFileParameters(const std::shared_ptr<ReadBuffer> &read_buffer, uint64_t file_size, MapFileInfoBuilder &mapFileInfoBuilder);

    private:
        /**
         * Contains all fields and optional fields that are contained in the .map file header.
         */
        std::shared_ptr<MapFileInfo> _map_file_info;

        /**
         * Contains zoom interval configuration.
         */
        std::vector<SubFileParameters> _sub_file_parameters;

        const std::string _tag = "MapFileHeader";

        const std::shared_ptr<mvt::Logger> _logger;
    };
}


#endif //CARTO_MOBILE_SDK_MAPFILEHEADER_H
