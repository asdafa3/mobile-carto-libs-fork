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

#ifndef CARTO_MOBILE_SDK_MFCONSTANTS_H
#define CARTO_MOBILE_SDK_MFCONSTANTS_H

#include <string>
#include <stdint.h>

namespace carto {

    /**
     * Class providing constants used when working with Mapsforge files.
     */
    class MFConstants {
    public:

        // Map file header

        /**
         * The string representation contained in the first 20 Bytes of each .map file to check if the binary
         * file is actually a .map file.
         */
        static const std::string _BINARY_OSM_MAGIC_BYTE;

        /**
         * String constant indicating mercator projection.
         */
        static const std::string _MERCATOR;

        /**
         * The maximum allowed size of a .map file header segment.
         */
        static const int _HEADER_SIZE_MAX;

        static const int _MAX_BUFFER_SIZE;

        /**
         * The minimum allowed size of a .map file header segment.
         */
        static const int _HEADER_SIZE_MIN;

        /**
         * The minimum allowed file specification version of a .map file.
         */
        static const int _SUPPORTED_FILE_VERSION_MIN;

        /**
         * The maximum allowed file specification version of a .map file.
         */
        static const int _SUPPORTED_FILE_VERSION_MAX;


        /**
         * The maximum allowed base zoom level for a .map file.
         */
        static const int _BASE_ZOOM_LEVEL_MAX;

        /**
         * Length of the debug signature of the index segment.
         */
        static const int _SIGNATURE_LENGTH_INDEX;

        // Sub files

        /**
         * The size constant of one index entry. 5 Bytes.
         */
        static const uint8_t _BYTES_PER_INDEX_ENTRY;

        // Tags

        /**
         * Separator used for string representation of key value pairs.
         */
        static const std::string _KEY_VALUE_SEPARATOR;

        // Flag masks

        /**
         * Bitmask used to check the existence of a comment field in the metadata of a .map file header segment.
         */
        static const uint8_t _HEADER_BITMASK_COMMENT;

        /**
         * Bitmask used to check the existence of a creator field in the metadata of a .map file header segment.
         */
        static const uint8_t _HEADER_BITMASK_CREATED_BY;

        /**
         * Bitmask used to check if the file contains debug signatures.
         */
        static const uint8_t _HEADER_BITMASK_DEBUG;

        /**
         * Bitmask used to check the existence of a start position field in the metadata of a .map file header segment.
         */
        static const uint8_t _HEADER_BITMASK_START_POSITION;

        /**
         * Bitmask used to check the existence of a start zoom level field in the metadata of a .map file header segment.
         */
        static const uint8_t _HEADER_BITMASK_START_ZOOM_LEVEL;

        /**
         * Bitmask used to check the existence of a language preference field in the metadata of a .map file header segment.
         */
        static const uint8_t _HEADER_BITMASK_LANGUAGES_PREFERENCE;

        /**
         * Bitmask used to check the existence of a proposed max zoom level field in the metadata of a .map file header segment.
         */
        static const int _START_ZOOM_LEVEL_MAX;

        // Conversation constants

        /**
         * Conversation factor from micro-degrees to degrees.
         */
        static const double _CONVERSION_FACTOR;

        /**
         * Minimum latitude (in degree) supported by the pseudo mercator projection.
         */
        static const double _LATITUDE_MIN;

        /**
         * Maximum latitude (in degree) supported by the pseudo mercator projection.
         */
        static const double _LATITUDE_MAX;

        // Reading Map files

        /**
         * 39 Bit offset of tile in sub file (also counting optional debug information). Bitmasked is applied to 5Byte (40 Bit)
         * block index entry pointing to a block in a sub file.
         */
        static const uint64_t _BITMASK_INDEX_OFFSET;

        /**
         * Bitmask to mask 5 Byte long to obtain the first bit. 1 Bit to check if the tile is completely covered by water.
         */
        static const uint64_t _BITMASK_INDEX_WATER;

        static const uint8_t _DEFAULT_START_ZOOM_LEVEL;
        static const uint32_t _INDEX_CACHE_SIZE;

        /**
         * Bitmask to check poi feature flag for existence of a elevation field.
         */
        static const uint32_t _POI_FEATURE_ELEVATION;

        /**
         * Bitmask to check poi feature flag for existence of a house number field.
         */
        static const uint32_t _POI_FEATURE_HOUSE_NUMBER;

        /**
         * Bitmask to check poi feature flag for existence of a name field.
         */
        static const uint32_t _POI_FEATURE_NAME;

        /**
         * Bitmask to check poi feature flag for existence of a layer field.
         */
        static const uint32_t _POI_LAYER_BITMASK;

        /**
         * Layer shift by to obtain only layer values >= 0. Layers refer to osm layer tag values.
         */
        static const uint32_t _POI_LAYER_SHIFT;

        /**
         * Bitmask to check poi feature flag for existence of a elevation field.
         */
        static const uint32_t _POI_NUMBER_OF_TAGS_BITMASK;

        /**
         * Length of block debug signature.
         */
        static const uint8_t _SIGNATURE_LENGTH_BLOCK;

        /**
         * Length of poi debug signature.
         */
        static const uint8_t _SIGNATURE_LENGTH_POI;

        /**
         * Length of way debug signature.
         */
        static const uint8_t _SIGNATURE_LENGTH_WAY;

        /**
         * Tag key for house number tag.
         */
        static const std::string _TAG_KEY_HOUSE_NUMBER;

        /**
         * Tag key for name tag.
         */
        static const std::string _TAG_KEY_NAME;

        /**
         * Tag key for ref tag.
         */
        static const std::string _TAG_KEY_REF;

        /**
         * Tag key for elevation tag.
         */
        static const std::string _TAG_KEY_ELEVATION;

        /**
         * Bitmask to check way feature flag byte for existence of a data blocks number field.
         */
        static const uint32_t _WAY_FEATURE_DATA_BLOCKS_BYTE;

        /**
         * Bitmask to check way feature flag to verify if double delta encoding was used to encode the current way.
         */
        static const uint32_t _WAY_FEATURE_DOUBLE_DELTA_ENCODING;

        /**
         * Bitmask to check way feature flag byte for existence of a house number field.
         */
        static const uint32_t _WAY_FEATURE_HOUSE_NUMBER;

        /**
         * Bitmask to check way feature flag byte for existence of a label position (in micro degrees offset).
         */
        static const uint32_t _WAY_FEATURE_LABEL_POSITION;

        /**
         * Bitmask to check way feature flag byte for existence of a name field.
         */
        static const uint32_t _WAY_FEATURE_NAME;

        /**
         * Bitmask to check way feature flag byte for existence of a ref field.
         */
        static const uint32_t _WAY_FEATURE_REF;

        /**
         * Bitmask to check way feature flag byte for existence of a layer field.
         */
        static const uint32_t _WAY_LAYER_BITMASK;

        /**
         * Layer shift to obtain only layer values >= 0. Layers refer to osm layer tag values.
         */
        static const uint32_t _WAY_LAYER_SHIFT;

        static const uint32_t _WAY_NUMBER_OF_TAGS_BITMASK;

        /**
         * The default way filter distance. Distance in meters by which the bbox of a query tile is enlarged.
         */
        static const uint32_t _WAY_FILTER_DISTANCE;

        /**
         * Number of index entries inside of an index block in the index segment of a sub file.
         */
        static const uint8_t _INDEX_ENTRIES_PER_BLOCK;

        /**
         * Size of an index block in a sub file index segment as BytesPerIndexBlock * IndexEntriesPerBlock.
         */
        static const uint64_t _SIZE_OF_INDEX_BLOCK;
    };
}

#endif //CARTO_MOBILE_SDK_MFCONSTANTS_H
