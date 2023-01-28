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

#ifndef CARTO_MOBILE_SDK_REQUIREDFIELDS_H
#define CARTO_MOBILE_SDK_REQUIREDFIELDS_H

#include "mapsforge-reader/ReadBuffer.h"
#include "MapFileInfo.h"

#include "core/MapBounds.h"
#include "mapsforge-reader/model/Tag.h"
#include "utils/GeomUtils.h"

#include "mapnikvt/Logger.h"
#include <tinyformat.h>
#include <string>

namespace carto {

    class MapFileInfoBuilder;

    /**
     * Container for the required fields stored in .map file header segment.
     */
    class RequiredFields final {
    public:
        /**
         * Reads the bbox information contained in the header segment of the .map file. BBox is in EPSG:3857.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readBoundingBox(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Read the total file size in bytes.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readFileSize(const std::shared_ptr<ReadBuffer> &readBuffer, uint64_t fileSize, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Read the file version number.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readFileVersion(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Read the magic byte that indicates that a binary file is actually a Mapsforge binary file.
         *
         * @param readBuffer The buffer containing the map file data.
         */
        static void readMagicByte(const std::shared_ptr<ReadBuffer> &readBuffer);

        /**
         * Read the creation date of the file.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readMapDate(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Read all POI tags listed in the header file. Tags that are referenced in tiles.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readPoiTags(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Read all Way tags listed in the header file. Tags that are referenced in tiles.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readWayTags(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Reads the name of the projection used to create this .map file. Typically Mercator.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readProjectionName(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);

        /**
         * Size of header segment in bytes.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readRemainingHeader(const std::shared_ptr<ReadBuffer> &readBuffer);

        /**
         * Reads the tile size in pixels.
         *
         * @param readBuffer The buffer containing the map file data.
         * @param mapFileInfoBuilder Builder object to store the fields.
         */
        static void readTilePixelSize(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder);
    private:
        RequiredFields();

        const std::string _tag;
    };
}


#endif //CARTO_MOBILE_SDK_REQUIREDFIELDS_H
