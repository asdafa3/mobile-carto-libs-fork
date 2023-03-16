/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014-2015 Ludwig M Brinckmann
 * Copyright 2014-2020 devemux86
 * Copyright 2015-2016 lincomatic
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

#ifndef CARTO_MOBILE_SDK_MAPFILE_H
#define CARTO_MOBILE_SDK_MAPFILE_H

#include "ReadBuffer.h"
#include <mapsforgereader/header/MapFileHeader.h>
#include <mapsforgereader/model/MapQueryResult.h>
#include <mapsforgereader/QueryParameters.h>
#include <mapsforgereader/utils/ProjectionUtils.h>
#include <iostream>
#include <mutex>
#include <memory>

#include <mapnikvt/Logger.h>
#include <mapnikvt/Types.h>
#include <vt/TileTransformer.h>

namespace carto {
    namespace mapsforge {

        /**
         * Selector enumerator.
         * Used to query either pois or both ways and POIs.
         */
        enum class Selector {
            /**
             * POIs and Ways should be returned.
             */
            ALL,
            /**
             * Only POIs are returned.
             */
            POIS
        };

        /**
         * Provides functionality to read tile data form a map file.
         */
        class MapFile {
        public:

            
            
            

            

            /**
             * Construct a MapFile instance for a specific .map file specified by it path.
             * @param path_to_map_file The path of the map file in the app bundle.
             * @param tagFilter A Json based filter to filter Ways by their tags.
             */
            MapFile(const std::string &path_to_map_file, const std::shared_ptr<std::vector<Tag>> &tagFilter, std::shared_ptr<mvt::Logger> logger);

            virtual ~MapFile();

            /**
             * Return the bounds of the map file wrapped by this class.
             * Bounds min/max are in latitudes/longitudes.
             * @return The map bounds for the map file.
             */
            const MapBounds &getMapBounds() const;

            /**
             * Given an input tile query in XYZ coordinate scheme, reads all map data
             * for the query tile. This includes POIS and Ways.
             * @param tile An abstract tile information in form of coordinates in XYZ scheme.
             * @return A bundle of POIs and Way data
             */
            std::shared_ptr <MapQueryResult> readMapData(const mvt::MapTile &tile);

            /**
             * Given an input tile query in XYZ coordinate scheme, reads selected map data
             * for the query tile. This includes only POIS.
             * @param tile An abstract tile information in form of coordinates in XYZ scheme.
             * @return A bundle of POIs and no Ways.
             */
            std::shared_ptr <MapQueryResult> readPoiData(const mvt::MapTile &tile);

            /**
             * Checks if the requested tile is contained in the bounding box of the extent covered by the map file.
             * @param tile Tile definition in XYZ coordinates.
             * @return
             */
            bool containsTile(const mvt::MapTile &tile);

            void setTileTransformer(const std::shared_ptr<vt::TileTransformer>& tileTransformer);

        private:
            /**
             * Reads and processes all POIs of one block inside a .map file.
             *
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param numberOfPois The number of POIs that are contained on the query zoom level.
             * @param mapBounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
             * @param filterRequired If a bounding box check needs to be applied.
             * @param pois The result vector of POIs.
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return True if all data could successfully read.
             */
            bool
            processPois(const MapPos &tileOrigin, uint32_t numberOfPois, const MapBounds &mapBounds, bool filterRequired,
                        std::vector <POI> *pois, ReadBuffer &readBuffer);


            /**
             * Reads and processes all Ways of one block inside a .map file.
             *
             * @param queryParams Used as it contains an optional tile bitmask that can be used to filter ways.
             * @param numberOfWays The number of ways on the query zoom level.
             * @param mapBounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
             * @param filterRequired Indicates if ways need to be (potentially) filtered.
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param selector Specifies if way data should be added to result vector.
             * @param ways The result vector for Ways.
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return
             */
            bool processWays(const QueryParameters &queryParams, uint32_t numberOfWays, const MapBounds &mapBounds,
                             bool filterRequired,
                             const MapPos &tileOrigin, Selector selector, std::vector <Way> *ways, ReadBuffer &readBuffer);

            /**
             * Read and process one way data block of one way. Represents a segment of a composed way.
             *
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param doubleDelta Boolean indicating Single or double delta encoding strategy.
             * @param wayNodes The result vector of way nodes.
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return
             */
            bool
            processWayDataBlock(const MapPos &tileOrigin, bool doubleDelta, std::vector <std::vector<MapPos>> *wayNodes,
                                ReadBuffer &readBuffer);

            /**
             * Decodes one coordinate block of single delta encoded points. A coordinate block can either represent a LineString,
             * a polygon or a part of a nested polygon (with holes). Single delta encodes the offset
             * between points and with first point encoded as offset to the tile origin.
             *
             * @param waySegment The coordinate block to decode.
             * @param numberOfWayNodes The number of points this coordinate block is composed of.
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param readBuffer The readBuffer containing the data for the current block.
             */
            void
            decodeWayNodesSingleDelta(std::vector <MapPos> *waySegment, int numberOfWayNodes, const MapPos &tileOrigin, ReadBuffer &readBuffer);

            /**
             * Decodes one coordinate block of double delta encoded points. A coordinate block can either represent a LineString,
             * a polygon or a part of a nested polygon (with holes). Double delta encoded coordinates are encoded
             * from the tile origin as reference and encode the change of the offset between nodes.
             *
             * @param waySegment The coordinate block to decode.
             * @param numberOfWayNodes The number of points this coordinate block is composed of.
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param readBuffer The readBuffer containing the data for the current block.
             */
            void
            decodeWayNodesDoubleDelta(std::vector <MapPos> *waySegment, int numberOfWayNodes, const MapPos &tileOrigin, ReadBuffer &readBuffer);

            /**
             * Reads the position of a way label if the .map file is compiled to contain those.
             * The presence of a label for is a way is indicated by the feature flag byte of a way.
             *
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return Returns the encoded lat lon in micro degrees.
             */
            std::vector <int32_t> readOptionalLabelPosition(ReadBuffer &readBuffer);

            /**
             * Reads and processes one block of the range blocks relevant for a tile query.
             *
             * @param queryParams Contain information like query zoom level, range of blocks and an optional tile bitmask.
             * @param subFileParams The zoom interval information associated with a tile query.
             * @param mapBounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
             * @param tileOrigin Top left corner latitude and longitude in degrees (wgs84).
             * @param selector Indicator which kind of data should be queried.
             * @param bundle The result bundle of pois, or pois and ways.
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return
             */
            bool processSingleBlock(const QueryParameters &queryParams, const SubFileParameters &subFileParams,
                                    const MapBounds &mapBounds, const MapPos &tileOrigin, Selector selector,
                                    TileDataBundle *bundle,
                                    ReadBuffer &readBuffer);

            /**
             * Query data from a range of blocks corresponding to a tile request in tile XYZ coordinates.
             *
             * @param queryParams Contain information like query zoom level, range of blocks and an optional tile bitmask.
             * @param subFileParams The zoom interval information associated with a tile query.
             * @param mapBounds The map bounds with min/max position in latitudes and longitudes in degrees (wgs84).
             * @param selector Indicator which kind of data should be queried.
             * @return
             */
            std::shared_ptr <MapQueryResult>
            processBlocks(const QueryParameters &queryParams, const SubFileParameters &subFileParams,
                          const MapBounds &mapBounds, Selector selector);

            /**
             * Reads how many pois and ways are contained for a block based on the difference to the base
             * zoom level.
             *
             * @param subFileParams The zoom interval information associated with a tile query.
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return 2D vector containing storing <zoomlevel<#pois,#ways>>.
             */
            std::vector <std::vector<int>>
            readTileZoomTable(const SubFileParameters &subFileParams, ReadBuffer &readBuffer);

            /**
             * Reads the map data for a specific query tile in XYZ coordinates and uses a selector to
             * either only query pois or pois and ways for the .map file.
             *
             * @param tile Tile definition inf XYZ integer coordinates.
             * @param selector Indicator which kind of data should be queried.
             * @return
             */
            std::shared_ptr <MapQueryResult> readMapData(const mvt::MapTile &tile, Selector selector);

            /**
             * Checks the signature of a block. If the block has an invalid signature, it is
             * marked to be skipped. Signature is only availible for .map files compiled as debug.
             *
             * @param readBuffer The readBuffer containing the data for the current block.
             * @return True if block has a valid debug signature.
             */
            bool processBlockSignature(ReadBuffer &readBuffer);

            /**
             * Calculates the position of a block index entry inside the .map file from
             * the number of the desired block and reads
             * the pointer to the block. Block pointers are 5 Byte in size each.
             *
             * @param subFileParams The zoom interval information associated with a tile query.
             * @param blockNumber Single number indicating the target block.
             * @return File absolute offset to block as long integer.
             */
            uint64_t readBlockIndex(const SubFileParameters &subFileParams, long blockNumber);

        private:
            /**
             * The size of the wrapped file in bytes.
             */
            uint8_t _file_size;

            /**
             * The time stemp of the map file.
             */
            uint8_t _timestamp;

            /**
             * Initial min zoom level.
             */
            uint8_t _zoom_level_min = 0;

            /**
             * Initial max zoom level.
             */
            uint8_t _zoom_level_max = 255;

            /**
             * The path to the .map file wrapped.
             */
            std::string _filePath;

            /**
             * The total size of the .map file.
             */
            unsigned long _fileSize = 0;

            /**
             * Shared pointer to a vector of tag attributes used for
             * filtering based on tags.
             */
            std::shared_ptr <std::vector<Tag>> _tagFilter;

            /**
             * The projection used for projection related operations during the process.
             * Projection is EPSG:3857.
             */
            std::shared_ptr <Projection> _projection;

            /**
             * Object containing the meta data stored in the file header.
             * Includes various attributes along with a vector of tags
             * and definition of sub files and corresponding file pointers.
             */
            MapFileHeader _map_file_header;

            /**
             * @brief TileTransformer to transform tiles based on a projection method.
             * 
             */
            std::shared_ptr<vt::TileTransformer> _tileTransformer;

            /**
             * Mutex object to block thread when reading from a .map file.
             */
            mutable std::mutex _mutex;

            const std::string _tag = "MapFile";

            const std::shared_ptr<mvt::Logger> _logger;
        };
    }
}



#endif //CARTO_MOBILE_SDK_MAPFILE_H
