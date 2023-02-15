/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014 Ludwig M Brinckmann
 * Copyright 2015 devemux86
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

#ifndef CARTO_MOBILE_SDK_OPTIONALFIELDS_H
#define CARTO_MOBILE_SDK_OPTIONALFIELDS_H

#include "mapsforge-reader/ReadBuffer.h"

#include <mapnikvt/Types.h>
#include "mapnikvt/Logger.h"

#include <tinyformat.h>
#include <memory>

namespace carto {

    class MapFileInfoBuilder;

    /**
     * Container for the optional fields stored in .map file header segment.
     */
    class OptionalFields {
    public:

        using MapPos = carto::mvt::MapPos;
        using Logger = carto::mvt::Logger;

        /**
         * Constructs a class to read and parse optional fields contained in the header segment of a .map file.
         * The flag contained in the header segment needs to be known in advance to determine existence of optional fields.
         * Field existence is determined b bitmasking the flags byte.
         *
         * @param flags Flags byte indicating existence of optional fields.
         */
        OptionalFields(uint8_t flags, std::shared_ptr<Logger> logger);

        /**
         * Reads the optional fields of a Mapsforge .map binary file from the header meta data. Existence of individual optional
         * fields is marked by a flag byte (bit-masked with various predefined bitmasks). Writes the extracted fields to a
         * MapFileInfoBuilder object.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readOptionalFields(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        const std::string &getComment() const;

        const std::string &getCreatedBy() const;

        const std::string &getLanguagePreference() const;

        const MapPos &getStartPosition() const;

        const uint8_t &getStartZoomLevel() const;

        const bool &getIsDebugFile() const;
    private:

        /**
         * Reads language preference string.
         *
         * @param readBuffer The buffer containing the map file data.
         */
        void readLanguagePreference(const std::shared_ptr<ReadBuffer> &readBuffer);

        /**
         * Read the map start positon in lat/lon degrees if existent.
         *
         * @param readBuffer The buffer containing the map file data.
         */
        void readMapStartPosition(const std::shared_ptr<ReadBuffer> &readBuffer);

        /**
         * Read the start zoom level if existent.
         *
         * @param readBuffer The buffer containing the map file data.
         */
        void readMapStartZoomLevel(const std::shared_ptr<ReadBuffer> &readBuffer);

        /**
         * Read optional fields from header data stored in readBuffer.
         *
         *
         * @param readBuffer The buffer containing the map file data.
         */
        void readOptionalFields(const std::shared_ptr<ReadBuffer> &readBuffer);
    private:
        std::string _comment;
        std::string _created_by;
        std::string _lang_preference;
        MapPos _start_position;
        uint8_t _start_zoom_level;
        bool _is_debug_file;

        bool _has_comment;
        bool _has_created_by;
        bool _has_language_preference;
        bool _has_start_zoom_level;
        bool _has_start_position;

        const std::string _tag = "OptionalFields";

        const std::shared_ptr<Logger> _logger;
    };
}


#endif //CARTO_MOBILE_SDK_OPTIONALFIELDS_H
