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

#include "RequiredFields.h"
#include "components/Exceptions.h"
#include "mapsforge-reader/utils/LatLongUtils.h"
#include "mapsforge-reader/MFConstants.h"
#include "projections/EPSG3857.h"

namespace carto {

    void RequiredFields::readBoundingBox(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        double minLat = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
        double minLon = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
        double maxLat = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
        double maxLon = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
        MapPos min(minLon, minLat);
        MapPos max(maxLon, maxLat);
        EPSG3857 proj {};
        MapBounds bounds(proj.fromWgs84(min), proj.fromWgs84(max));
        mapFileInfoBuilder._bounding_box = bounds;
    }

    void RequiredFields::readFileSize(const std::shared_ptr<ReadBuffer> &readBuffer, uint64_t fileSize, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint64_t headerFileSize = readBuffer -> read_long();
        if (headerFileSize != fileSize) {
            Log::Errorf("RequiredFields::readFileSize: Invalid file size: %d", headerFileSize);
            throw GenericException("RequiredFields::readFileSize: Invalid file size");
        }
        mapFileInfoBuilder._file_size = headerFileSize;
    }

    void RequiredFields::readFileVersion(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint32_t fileVersion = readBuffer -> read_int();
        if (fileVersion < MFConstants::_SUPPORTED_FILE_VERSION_MIN || fileVersion > MFConstants::_SUPPORTED_FILE_VERSION_MAX) {
            Log::Errorf("RequiredFields::readFileVersion: Unsupported file version: %d", fileVersion);
            throw GenericException("RequiredFields::readFileVersion: Unsupported file version");
        }
        mapFileInfoBuilder._file_version = fileVersion;
    }

    void RequiredFields::readMagicByte(const std::shared_ptr<ReadBuffer> &readBuffer) {
        int magicByteLength = MFConstants::_BINARY_OSM_MAGIC_BYTE.length();
        if (!(readBuffer -> readFromFile(magicByteLength + 4))) {
            Log::Error("RequiredFields::readMagicByte: Reading magic byte has failed");
            throw GenericException("RequiredFields::readMagicByte: Could not read magic byte");
        }
        std::string magicByte = readBuffer -> read_utf8(magicByteLength);
        Log::Infof("magicByteLength: %d", magicByte.length());
        if (!(magicByte == MFConstants::_BINARY_OSM_MAGIC_BYTE)) {
            Log::Errorf("RequiredFields::readMagicByte: Invalid magic byte : %s", magicByte);
            throw GenericException("RequiredFields::readMagicByte: Invalid magic byte");
        }
    }

    void RequiredFields::readMapDate(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint64_t mapDate = readBuffer -> read_long();
        // is the map date before 2010-01-10 ?
        if (mapDate < 1200000000000L) {
            Log::Errorf("RequiredFields::readMapDate: Invalid map date: %d", mapDate);
            throw GenericException("RequiredFields::readMapDate: Invalid map date");
        }
        mapFileInfoBuilder._map_date = mapDate;
    }

    void RequiredFields::readPoiTags(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint16_t numberOfPoiTags = readBuffer -> read_short();
        if (numberOfPoiTags < 0) {
            Log::Errorf("RequiredFields::readPoiTags: Invalid number of poi tags: %d", numberOfPoiTags);
            throw GenericException("RequiredFields::readPoiTags: Invalid number of poi tags");
        }

        std::vector<Tag> tags;
        tags.reserve(numberOfPoiTags);
        for (int currentTagId = 0; currentTagId < numberOfPoiTags; ++currentTagId) {
            std::string tagName = readBuffer -> read_utf8();
            if (tagName.empty()) {
                throw GenericException("RequiredFields::readPoiTags: Poi tag must be not empty!");
            }
            Tag tag(tagName);
            tags.push_back(tag);
        }

        mapFileInfoBuilder._poi_tags = tags;
    }

    void RequiredFields::readWayTags(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint16_t numberOfWayTags = readBuffer -> read_short();
        if (numberOfWayTags < 0) {
            Log::Errorf("RequiredFields::readWayTags: Invalid number of way tags: %d", numberOfWayTags);
            throw GenericException("RequiredFields::readWayTags: Invalid number of way tags");
        }

        std::vector<Tag> tags;
        tags.reserve(numberOfWayTags);

        for (int currentTagId = 0; currentTagId < numberOfWayTags; ++currentTagId) {
            std::string tagName = readBuffer -> read_utf8();
            if (tagName.empty()) {
                throw GenericException("RequiredFields::readWayTags: Way tag must not be empty!");
            }
            Tag tag(tagName);
            tags.push_back(tag);
        }

        mapFileInfoBuilder._way_tags = tags;
    }

    void RequiredFields::readProjectionName(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        std::string projectionName = readBuffer -> read_utf8();
        if (!(projectionName == MFConstants::_MERCATOR)) {
            Log::Errorf("RequiredFields::readProjectionName: Unsupported projection type: %s", projectionName);
            throw GenericException("RequiredFields::readProjectionName: Unsupported projection type");
        }
        mapFileInfoBuilder._projection_name = projectionName;
    }

    void RequiredFields::readRemainingHeader(const std::shared_ptr<ReadBuffer> &readBuffer) {
        uint32_t remainingHeaderSize = readBuffer -> read_int();
        if (remainingHeaderSize < MFConstants::_HEADER_SIZE_MIN || remainingHeaderSize > MFConstants::_HEADER_SIZE_MAX) {
            Log::Errorf("RequiredFields::readRemainingHeader: Invalid remaining header size: %d", remainingHeaderSize);
            throw GenericException("Invalid remaining header size");
        }

        // if valid, read rest of header
        if (!(readBuffer -> readFromFile(MFConstants::_BINARY_OSM_MAGIC_BYTE.length() + 4, remainingHeaderSize))) {
            Log::Errorf("RequiredFields::readRemainingHeader: Reading header data has failed: %d", remainingHeaderSize);
            throw GenericException("Reading header data has failed");
        }
    }

    void RequiredFields::readTilePixelSize(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint16_t tileSize = readBuffer -> read_short();
        mapFileInfoBuilder._tile_pixel_size = tileSize;
    }
}
