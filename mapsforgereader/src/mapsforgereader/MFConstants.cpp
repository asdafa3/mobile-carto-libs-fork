/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2015-2020 devemux86
 * Copyright 2016 bvgastel
 * Copyright 2017 linuskr
 * Copyright 2017 Gustl22
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

#include "MFConstants.h"

namespace carto {
    // Map file header

    const std::string MFConstants::_BINARY_OSM_MAGIC_BYTE = "mapsforge binary OSM";
    const std::string MFConstants::_MERCATOR = "Mercator";

    const int MFConstants::_HEADER_SIZE_MAX = 1000000;
    const int MFConstants::_MAX_BUFFER_SIZE = 10000000;
    const int MFConstants::_HEADER_SIZE_MIN = 70;

    const int MFConstants::_SUPPORTED_FILE_VERSION_MIN = 3;
    const int MFConstants::_SUPPORTED_FILE_VERSION_MAX = 5;


    const int MFConstants::_BASE_ZOOM_LEVEL_MAX = 20;
    const int MFConstants::_SIGNATURE_LENGTH_INDEX = 16;

    // Sub files

    const uint8_t MFConstants::_BYTES_PER_INDEX_ENTRY = 5;
    const uint8_t MFConstants::_INDEX_ENTRIES_PER_BLOCK = 128;
    const uint64_t MFConstants::_SIZE_OF_INDEX_BLOCK = _INDEX_ENTRIES_PER_BLOCK * _BYTES_PER_INDEX_ENTRY;

    // Tags

    const std::string MFConstants::_KEY_VALUE_SEPARATOR = "=";

    // Flag masks

    const uint8_t MFConstants::_HEADER_BITMASK_COMMENT = 0x08;
    const uint8_t MFConstants::_HEADER_BITMASK_CREATED_BY = 0x04;
    const uint8_t MFConstants::_HEADER_BITMASK_DEBUG = 0x80;
    const uint8_t MFConstants::_HEADER_BITMASK_START_POSITION = 0x40;
    const uint8_t MFConstants::_HEADER_BITMASK_START_ZOOM_LEVEL = 0x20;
    const uint8_t MFConstants::_HEADER_BITMASK_LANGUAGES_PREFERENCE = 0x10;

    const int MFConstants::_START_ZOOM_LEVEL_MAX = 21;

    // Conversation constants

    const double MFConstants::_CONVERSION_FACTOR = 1000000.0;
    const double MFConstants::_LATITUDE_MIN = -_LATITUDE_MAX;
    const double MFConstants::_LATITUDE_MAX = 85.05112877980659;

    // Reading map files

    const uint64_t MFConstants::_BITMASK_INDEX_OFFSET = 0x7FFFFFFFFFL;
    const uint64_t MFConstants::_BITMASK_INDEX_WATER = 0x8000000000L;

    const uint8_t MFConstants::_DEFAULT_START_ZOOM_LEVEL = 12;
    const uint32_t MFConstants::_INDEX_CACHE_SIZE = 64;

    const uint32_t MFConstants::_POI_FEATURE_ELEVATION = 0x20;
    const uint32_t MFConstants::_POI_FEATURE_HOUSE_NUMBER = 0x40;
    const uint32_t MFConstants::_POI_FEATURE_NAME = 0x80;
    const uint32_t MFConstants::_POI_LAYER_BITMASK = 0xf0;
    const uint32_t MFConstants::_POI_LAYER_SHIFT = 4;
    const uint32_t MFConstants::_POI_NUMBER_OF_TAGS_BITMASK = 0x0f;

    const uint8_t MFConstants::_SIGNATURE_LENGTH_BLOCK = 32;
    const uint8_t MFConstants::_SIGNATURE_LENGTH_POI = 32;
    const uint8_t MFConstants::_SIGNATURE_LENGTH_WAY = 32;

    const std::string MFConstants::_TAG_KEY_HOUSE_NUMBER = "addr:housenumber";
    const std::string MFConstants::_TAG_KEY_NAME = "name";
    const std::string MFConstants::_TAG_KEY_REF = "ref";
    const std::string MFConstants::_TAG_KEY_ELEVATION = "ele";

    const uint32_t MFConstants::_WAY_FEATURE_DATA_BLOCKS_BYTE = 0x08;
    const uint32_t MFConstants::_WAY_FEATURE_DOUBLE_DELTA_ENCODING = 0x04;
    const uint32_t MFConstants::_WAY_FEATURE_HOUSE_NUMBER = 0x40;
    const uint32_t MFConstants::_WAY_FEATURE_LABEL_POSITION = 0x10;
    const uint32_t MFConstants::_WAY_FEATURE_NAME = 0x80;
    const uint32_t MFConstants::_WAY_FEATURE_REF = 0x20;
    const uint32_t MFConstants::_WAY_LAYER_BITMASK = 0xf0;
    const uint32_t MFConstants::_WAY_LAYER_SHIFT = 4;
    const uint32_t MFConstants::_WAY_NUMBER_OF_TAGS_BITMASK = 0x0f;
    const uint32_t MFConstants::_WAY_FILTER_DISTANCE = 20;
}