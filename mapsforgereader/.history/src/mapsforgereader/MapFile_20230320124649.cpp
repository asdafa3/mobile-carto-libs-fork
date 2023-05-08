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

#include <mapsforgereader/MapFile.h>
#include <mapsforgereader/ReadBuffer.h>
#include <mapsforgereader/MFConstants.h>
#include <mapsforgereader/QueryParameters.h>
#include <mapsforgereader/Deserializer.h>
#include "utils/TileUtils.h"
#include <mapsforgereader/utils/LatLongUtils.h>
#include <mapsforgereader/utils/Converter.h>
#include <mapsforgereader/utils/ProjectionUtils.h>
#include <mapsforgereader/utils/Converter.h>

#include <fstream>
#include <iterator>
#include <iostream>
#include <vector>
#include <climits>
#include <math.h>
#include <stdexcept>

namespace carto {
    namespace mapsforge {

        MapFile::MapFile(const std::string &path_to_map_file, const std::shared_ptr <std::vector<Tag>> &tagFilter, std::shared_ptr<mvt::Logger> logger) :
                _filePath(path_to_map_file),
                _tileTransformer(),
                _projection(std::make_shared<EPSG3857Projection>()),
                _map_file_header(std::move(logger)),
                _logger(std::move(logger)) {
            try {
                std::shared_ptr <ReadBuffer> readBuffer = std::make_shared<ReadBuffer>(path_to_map_file);
                _fileSize = readBuffer->getCurrentFileSize();
                _tagFilter = tagFilter;
                // Read in header data for a map file. Header contains information about where tiles are stored in the file.
                _map_file_header.readHeader(readBuffer, _fileSize);
            } catch (const /*carto::FileException*/ std::runtime_error &ex) {
                // TODO: Throw exception©
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Could not extract information from map file. %s", _tag, ex.what()));
                // Log::Errorf("Could not extract information from map file. ", ex.what());
            }
        }

        MapFile::~MapFile() {}

        const MapFile::MapBounds2D &MapFile::getMapBounds() const {
            return _map_file_header.getMapFileInfo()->getBoundingBox();
        }

        std::shared_ptr <MapQueryResult> MapFile::readPoiData(const MapTile &tile) {
            return readMapData(tile, Selector::POIS);
        }

        std::shared_ptr <MapQueryResult> MapFile::readMapData(const MapTile &tile) {
            return readMapData(tile, Selector::ALL);
        }

        void MapFile::setTileTransformer(const std::shared_ptr<vt::TileTransformer>& tileTransformer) {
            std::lock_guard<std::mutex> lock(_mutex);
            _tileTransformer = tileTransformer;
        }

        bool MapFile::containsTile(const MapTile &tile) {
            // projection is EPSG3857
            MapBounds3D bboxTile3D = _tileTransformer->calculateTileBBox(tile);
            MapBounds2D bboxTile2D = Converter::bbox3tobbox2(bboxTile3D);
            bboxTile2D.min = MapPos(bboxTile2D.min(0), -bboxTile2D.min(1));
            bboxTile2D.max = MapPos(bboxTile2D.max(0), -bboxTile2D.max(1));
            MapBounds2D bboxMapFile = _map_file_header.getMapFileInfo()->getBoundingBox();

            // check if tile bounding box is inside or interecting with the map file bbox
            MapBounds2D bboxIntersection = bboxTile2D.intersect(bboxMapFile);
            bool intersects = !bboxIntersection.empty();

            // check if the .map file supports the zoom level of the query tile
            bool zoomCondLow = tile.zoom >= _map_file_header.getMapFileInfo()->getMinZoomLevel();
            bool zoomCondHigh = tile.zoom <= _map_file_header.getMapFileInfo()->getMaxZoomLevel();
            bool zoomInRange = zoomCondLow && zoomCondHigh;

            return intersects && zoomInRange;
        }

        std::shared_ptr <MapQueryResult> MapFile::readMapData(const MapTile &tile, Selector selector) {
            QueryParameters queryParams{};
            queryParams.setQueryZoomLevel(_map_file_header.getQueryZoomLevel(tile.zoom));

            SubFileParameters subFileParameter = _map_file_header.getSubFileParameters(queryParams.getQueryZoomLevel());
            // calculate tiles covered by the query tile on the base zoom level
            queryParams.calculateTiles(tile, subFileParameter);
            // calculate corresponding blocks in .map file from the previosly calculated tiles
            queryParams.calculateBlocks(subFileParameter);

            MapTile flippedTile(tile.zoom, tile.x, -tile.y);
            // calculate tile origin for decoding coordinates later
            MapBounds3D projectedMapBounds3D = _tileTransformer->calculateTileBBox(flippedTile);
            MapBounds2D projectedMapBounds2D = Converter::bbox3tobbox2(projectedMapBounds3D);
            // MapBounds projectedMapBounds = TileUtils::CalculateMapTileBounds(tile.getFlipped(), _projection);
            MapPos projectedMin = _projection->toWgs84(projectedMapBounds2D.min);
            MapPos projectedMax = _projection->toWgs84(projectedMapBounds2D.max);
            MapBounds2D latLonBounds(projectedMin, projectedMax);

            return processBlocks(queryParams, subFileParameter, latLonBounds, selector);
        }

        std::shared_ptr <MapQueryResult>
        MapFile::processBlocks(const QueryParameters &queryParams, const SubFileParameters &subFileParams,
                               const MapBounds2D &mapBounds, Selector selector) {

            MapQueryResult mapQueryResult{};

            for (uint64_t row = queryParams.getFromBlockY(); row <= queryParams.getToBlockY(); ++row) {
                for (uint64_t col = queryParams.getFromBlockX(); col <= queryParams.getToBlockX(); ++col) {

                    // calc block number in the file
                    uint64_t blockNumber = row * subFileParams.getBlocksWidth() + col;

                    // get data index entry
                    uint64_t currentBlockIndexEntry = readBlockIndex(subFileParams, blockNumber);

                    // check if block pointer is in valid range
                    uint64_t currentBlockPointer = currentBlockIndexEntry & MFConstants::_BITMASK_INDEX_OFFSET;
                    if (currentBlockPointer < 1 || currentBlockPointer > subFileParams.getSubFileSize()) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid current block pointer: %d", _tag, currentBlockPointer));
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Sub file size: %d", _tag, subFileParams.getSubFileSize()));
                        //Log::Warnf("MapFile::processBlocks: Invalid current block pointer: %d", currentBlockPointer);
                        //Log::Warnf("MapFile::processBlocks: Sub file size: %d", subFileParams.getSubFileSize());
                        return std::shared_ptr<MapQueryResult>();
                    }

                    uint64_t nextBlockPointer;

                    // check if next block is the last block in the sub file
                    if (blockNumber + 1 == subFileParams.getNumberOfBlocks()) {
                        nextBlockPointer = subFileParams.getSubFileSize();
                    } else {
                        // read the position of the next block in the sub file
                        nextBlockPointer = readBlockIndex(subFileParams, blockNumber + 1) & MFConstants::_BITMASK_INDEX_OFFSET;
                        if (nextBlockPointer > subFileParams.getSubFileSize()) {
                            _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid next block pointer: %d", _tag, nextBlockPointer));
                            _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Sub file size: %d", _tag, subFileParams.getSubFileSize()));
                            // Log::Warnf("MapFile::processBlocks: Invalid next block pointer: %d", nextBlockPointer);
                            // Log::Warnf("MapFile::processBlocks: Sub file size: %d", subFileParams.getSubFileSize());
                            return std::shared_ptr<MapQueryResult>();
                        }
                    }

                    // calculate the amount of bytes for the current block
                    uint32_t currentBlockSize = (uint32_t)(nextBlockPointer - currentBlockPointer);
                    if (currentBlockSize < 0) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Current block size must not be negative: %d", _tag, currentBlockSize));
                        // Log::Warnf("MapFile::processBlocks: Current block size must not be negative: %d", currentBlockSize);
                        return std::shared_ptr<MapQueryResult>();
                    } else if (currentBlockSize == 0) {
                        // the current block is empty, continue with next block
                        continue;
                    } else if (currentBlockSize > 10000000) {
                        // current block is too large, continue
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Current block is too large: %d", _tag, currentBlockSize));
                        // Log::Warnf("MapFile::processBlocks: Current block is too large: %d", currentBlockSize);
                        continue;
                    } else if (currentBlockPointer + currentBlockSize > _fileSize) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Current block exceeds file size: %d", _tag, currentBlockSize));
                        // Log::Warnf("MapFile::processBlocks: Current block exceeds file size: %d", currentBlockSize);
                        return std::shared_ptr<MapQueryResult>();
                    }

                    _mutex.lock();
                    ReadBuffer readBuffer(_filePath, _logger);
                    // fill the buffer with the data of the current block
                    if (!readBuffer.readFromFile(subFileParams.getStartAddress() + currentBlockPointer, currentBlockSize)) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Reading block has failed: %d", _tag, currentBlockSize));
                        // Log::Warnf("MapFile::processBlocks: Reading block has failed: %d", currentBlockSize);
                        return std::shared_ptr<MapQueryResult>();
                    }
                    _mutex.unlock();

                    MapTile boundaryTileTopLeft(
                        subFileParams.getBaseZoomLevel(),
                        subFileParams.getBoundaryTileLeft() + col,
                        subFileParams.getBoundaryTileTop() + row
                    );

                    MapTile flippedTile(boundaryTileTopLeft.zoom, boundaryTileTopLeft.x, -boundaryTileTopLeft.y);
                    MapBounds3D projectedMapBounds3D = _tileTransformer->calculateTileBBox(flippedTile);
                    MapBounds2D projectedMapBounds2D = Converter::bbox3tobbox2(projectedMapBounds3D);
                    // MapBounds min is bottom left (south-west), MapBounds max is top right (north-east).
                    // We need top-left position for further calculations. Coordinates are in WGS84.
                    MapPos projectedMin = _projection->toWgs84(projectedMapBounds2D.min);
                    MapPos projectedMax = _projection->toWgs84(projectedMapBounds2D.max);
                    // MapPos projectedMin = _projection->toWgs84(projectedMapBounds.min);
                    // MapPos projectedMax = _projection->toWgs84(projectedMapBounds.max);
                    MapPos topLeftPosition(projectedMin(1), projectedMax(2)); //FIXME: Check if this makes sense

                    try {
                        TileDataBundle bundle{};

                        // Read the data of a single block based on the selector used
                        if (processSingleBlock(queryParams, subFileParams, mapBounds, topLeftPosition, selector, &bundle, readBuffer)) {
                            mapQueryResult.add(bundle);
                        }
                    } catch (const std::runtime_error &ex) {
                        _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Error processing block! %s", _tag, ex.what()));
                        throw std::runtime_error(tfm::format("%s::Error processing block", _tag));
                    }
                }
            }

            return std::make_shared<MapQueryResult>(mapQueryResult);
        }

        bool MapFile::processWays(const QueryParameters &queryParams, uint32_t numberOfWays, const MapBounds &mapBounds,
                                  bool filterRequired, const MapPos &tileOrigin, Selector selector,
                                  std::vector <Way> *ways, ReadBuffer &readBuffer) {

            std::vector <Tag> wayTags = _map_file_header.getMapFileInfo()->getWayTags();

            // Extend the bounding box by a few meters to have filter crossing ways.
            // Defining bounding box positions are in wgs84 format before and in projected coordinates after.
            MapBounds2D wayFilterBox = LatLongUtils::enlarge(mapBounds, 100);

            for (uint32_t elemCounter = numberOfWays; elemCounter != 0; --elemCounter) {
                if (_map_file_header.getMapFileInfo()->getDebugInfo()) {
                    // get way signature
                    std::string waySignature = readBuffer.read_utf8(MFConstants::_SIGNATURE_LENGTH_WAY);
                    // check if way signature starts with token
                    if (!waySignature.rfind("---WayStart", 0)) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid way signature: %s", _tag, waySignature));
                        // Log::Warnf("MapFile::processWays: Invalid way signature: %s", waySignature);
                        return false;
                    }
                }

                // get way size (VBE-U)
                uint64_t wayDataSize = readBuffer.read_var_ulong();
                if (wayDataSize < 0) {
                    _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid way data size: %d", _tag, wayDataSize));
                    // Log::Warnf("MapFile::processWays: Invalid way data size: %d", wayDataSize);
                    return false;
                }

                if (queryParams.getUseTileBitmask()) {
                    // get way tile bitmask (2 Bytes)
                    uint16_t tileBitmask = readBuffer.read_short();
                    // check if way is inside the requested tile
                    if (!(queryParams.getQueryTileBitmask() & tileBitmask)) {
                        // skip rest of the way and continue with next way
                        readBuffer.skip_bytes(wayDataSize - 2);
                        continue;
                    }
                } else {
                    // ignore way tile bitmask
                    readBuffer.skip_bytes(2);
                }

                // get special byte that encodes way flags
                int8_t specialByte = readBuffer.read_byte();

                // Bit 1-4 encode the layer
                // FIXME: be careful here!
                auto layer = (int8_t)((uint8_t)(specialByte & MFConstants::_WAY_LAYER_BITMASK) >> MFConstants::_WAY_LAYER_SHIFT);
                // Bit 5-8 encode the number of tag IDs
                auto numberOfTags = (int8_t)(specialByte & MFConstants::_WAY_NUMBER_OF_TAGS_BITMASK);

                // get tags from IDs (VBE-U)
                std::vector <Tag> decodedTags;
                if (!(readBuffer.readTags(wayTags, &decodedTags, numberOfTags))) {
                    return false;
                }

                // get the feature bitmask (1 byte)
                int8_t featureByte = readBuffer.read_byte();

                // Bit 1-6 encode optional features
                bool featureName = (featureByte & MFConstants::_WAY_FEATURE_NAME) != 0;
                bool featureHouseNumber = (featureByte & MFConstants::_WAY_FEATURE_HOUSE_NUMBER) != 0;
                bool featureRef = (featureByte & MFConstants::_WAY_FEATURE_REF) != 0;
                bool featureLabelPosition = (featureByte & MFConstants::_WAY_FEATURE_LABEL_POSITION) != 0;
                bool featureWayDataBlocksByte = (featureByte & MFConstants::_WAY_FEATURE_DATA_BLOCKS_BYTE) != 0;
                bool featureWayDoubleDeltaEncoding = (featureByte & MFConstants::_WAY_FEATURE_DOUBLE_DELTA_ENCODING) != 0;

                // check if way has a name
                if (featureName) {
                    decodedTags.emplace_back(MFConstants::_TAG_KEY_NAME, readBuffer.read_utf8());
                }

                // check if way has a housenumber
                if (featureHouseNumber) {
                    decodedTags.emplace_back(MFConstants::_TAG_KEY_HOUSE_NUMBER, readBuffer.read_utf8());
                }

                // check if way has a reference
                if (featureRef) {
                    decodedTags.emplace_back(MFConstants::_TAG_KEY_REF, readBuffer.read_utf8());
                }

                std::vector <int32_t> labelPosition;
                if (featureLabelPosition) {
                    labelPosition = readOptionalLabelPosition(readBuffer);
                }

                uint64_t numberWayDataBlocks = 1;
                if (featureWayDataBlocksByte) {
                    numberWayDataBlocks = readBuffer.read_var_ulong();
                }

                if (numberWayDataBlocks < 1) {
                    _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid number of way data blocks: %d", _tag, numberWayDataBlocks));
                    // Log::Warnf("MapFile::processWays: Invalid number of way data blocks: %d", numberWayDataBlocks);
                    return false;
                }

                // process all way data blocks (spatially partitioned geometries)
                for (auto wayDataBlock = 0; wayDataBlock < numberWayDataBlocks; ++wayDataBlock) {
                    std::vector <std::vector<MapPos>> wayNodes;
                    // process one single way data block
                    if (processWayDataBlock(tileOrigin, featureWayDoubleDeltaEncoding, &wayNodes, readBuffer)) {

                        // filter way block if way intersects with buffered bbox
                        if (filterRequired && !LatLongUtils::intersectsArea(wayFilterBox, wayNodes)) {
                            continue;
                        }
                        if (selector == Selector::ALL || featureName || featureHouseNumber || featureRef) {
                            MapPos labelLatLon{};

                            // decode way label if existing
                            if (!labelPosition.empty()) {
                                labelLatLon(1) = wayNodes[0][0](1) + LatLongUtils::microdegreesToDegrees(labelPosition[1]);
                                labelLatLon(2) = wayNodes[0][0](2) + LatLongUtils::microdegreesToDegrees(labelPosition[0]);
                            }

                            // apply tag filter. ways must contain one of the tags.
                            if (_tagFilter) {
                                bool containsTag = std::find_first_of(decodedTags.begin(), decodedTags.end(), _tagFilter->begin(), _tagFilter->end()) != decodedTags.end();

                                // If no tag is contained, dont add the feature
                                if (!containsTag) {
                                    continue;
                                }
                            }

                            ways->push_back(Way(layer, decodedTags, wayNodes, labelLatLon));
                        }
                    }
                }
            }

            return true;
        }

        bool MapFile::processWayDataBlock(const MapPos &tileOrigin, bool doubleDelta,
                                          std::vector <std::vector<MapPos>> *wayNodes, ReadBuffer &readBuffer) {
            // get and check the number of way coordinate blocks (VBE-U)
            uint64_t numberWayCoordinateBlocks = readBuffer.read_var_ulong();
            wayNodes->reserve(numberWayCoordinateBlocks);

            if (numberWayCoordinateBlocks < 1 || numberWayCoordinateBlocks > SHRT_MAX) {
                _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid number of way coordinate blocks: %d", _tag, numberWayCoordinateBlocks));
                // Log::Warnf("MapFile::processWayDataBlock: Invalid number of way data blocks: %d", numberWayCoordinateBlocks);
                return false;
            }

            // read way coordinate blocks
            for (auto coordinateBlock = 0; coordinateBlock < numberWayCoordinateBlocks; ++coordinateBlock) {
                // get and check number of way nodes (VBE-U)
                uint64_t numberWayNodes = readBuffer.read_var_ulong();

                if (numberWayNodes < 2 || numberWayNodes > SHRT_MAX) {
                    _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid number of way nodes: %d", _tag, numberWayNodes));
                    //Log::Warnf("MapFile::processWayDataBlock: Invalid number of way nodes: %d", numberWayNodes);
                    return false;
                }

                // current way segment
                std::vector <MapPos> waySegment;
                waySegment.reserve(numberWayNodes);

                // decode way based on the encoding specified for it
                if (doubleDelta) {
                    decodeWayNodesDoubleDelta(&waySegment, numberWayNodes, tileOrigin, readBuffer);
                } else {
                    decodeWayNodesSingleDelta(&waySegment, numberWayNodes, tileOrigin, readBuffer);
                }

                wayNodes->push_back(waySegment);
            }

            return true;
        }

        void MapFile::decodeWayNodesDoubleDelta(std::vector <MapPos> *waySegment, int numberOfWayNodes, const MapPos &tileOrigin, ReadBuffer &readBuffer) {
            // get first way node latitude offset (VBE-S)
            double wayNodeLatitude = tileOrigin(2) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());
            // get first way node longitude offset (VBE-S)
            double wayNodeLongitude = tileOrigin(1) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

            // store first way node
            waySegment->push_back({ wayNodeLongitude, wayNodeLatitude });

            double previousSingleDeltaLatitude;
            double previousSingleDeltaLongitude;

            for (auto wayNodesIndex = 1; wayNodesIndex < numberOfWayNodes; ++wayNodesIndex) {
                // get the way node latitude double-delta offset (VBE-S)
                double doubleDeltaLatitude = LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

                // get the way node longitude double-delta offset (VBE-S)
                double doubleDeltaLongitude = LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

                double singleDeltaLatitude = doubleDeltaLatitude + previousSingleDeltaLatitude;
                double singleDeltaLongitude = doubleDeltaLongitude + previousSingleDeltaLongitude;

                wayNodeLatitude += singleDeltaLatitude;
                wayNodeLongitude += singleDeltaLongitude;

                // Decoding near international date line can return values slightly outside valid [-180°, 180°] due to calculation precision
                if (wayNodeLongitude < -180.0 && (-180.0 - wayNodeLongitude) < 0.001) {
                    wayNodeLongitude = -180.0;
                } else if (wayNodeLongitude > 180.0 && (wayNodeLongitude - 180.0) < 0.001) {
                    wayNodeLongitude = 180.0;
                }

                waySegment->push_back({ wayNodeLongitude, wayNodeLatitude });

                previousSingleDeltaLatitude = singleDeltaLatitude;
                previousSingleDeltaLongitude = singleDeltaLongitude;
            }
        }

        void MapFile::decodeWayNodesSingleDelta(std::vector <MapPos> *waySegment, int numberOfWayNodes, const MapPos &tileOrigin, ReadBuffer &readBuffer) {
            // get first way node latitude offset (VBE-S)
            double wayNodeLatitude = tileOrigin(2) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());
            // get first way node longitude offset (VBE-S)
            double wayNodeLongitude = tileOrigin(1) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

            // store first way node
            waySegment->push_back({ wayNodeLongitude, wayNodeLatitude });

            for (auto wayNodesIndex = 1; wayNodesIndex < numberOfWayNodes; ++wayNodesIndex) {
                // get way node latitude offset (VBE-s)
                wayNodeLatitude += LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());
                // get way node longitude offset (VBE-S)
                wayNodeLongitude += LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

                // Decoding near international date line can return values slightly outside valid [-180°, 180°] due to calculation precision
                if (wayNodeLongitude < -180.0 && (-180.0 - wayNodeLongitude) < 0.001) {
                    wayNodeLongitude = -180.0;
                } else if (wayNodeLongitude > 180.0 && (wayNodeLongitude - 180.0) < 0.001) {
                    wayNodeLongitude = 180.0;
                }

                waySegment->push_back({ wayNodeLongitude, wayNodeLatitude });
            }
        }

        std::vector <int32_t> MapFile::readOptionalLabelPosition(ReadBuffer &readBuffer) {
            std::vector <int32_t> labelPosition(2);

            // get label position latitude offset (microdegrees) (VBE-S)
            labelPosition[1] = readBuffer.read_var_long();
            // get label position longitude offset (microdegrees) (VBE-S)
            labelPosition[0] = readBuffer.read_var_long();

            return labelPosition;
        }

        bool MapFile::processPois(const MapPos &tileOrigin, uint32_t numberOfPois, const MapBounds2D &mapBounds, bool filterRequired, std::vector <POI> *pois, ReadBuffer &readBuffer) {
            std::vector <Tag> poiTags = _map_file_header.getMapFileInfo()->getPoiTags();

            for (uint32_t elementCounter = numberOfPois; elementCounter != 0; --elementCounter) {
                if (_map_file_header.getMapFileInfo()->getDebugInfo()) {
                    // check poi signature
                    std::string poiSignature = readBuffer.read_utf8(MFConstants::_SIGNATURE_LENGTH_POI);
                    if (!poiSignature.rfind("***POIStart", 0)) {
                        _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid POI signature: %s", _tag, poiSignature));
                        // Log::Warnf("MapFile::processPois: Invalid POI signature: %s", poiSignature);
                        return false;
                    }
                }

                // POI latitude as (VBE-S)
                double lat = tileOrigin(2) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());
                // POI longitude as (VBE-S)
                double lon = tileOrigin(1) + LatLongUtils::microdegreesToDegrees(readBuffer.read_var_long());

                // get special byte which encodes flags
                int8_t specialByte = readBuffer.read_byte();

                // Bit 1-4 represent layer
                int8_t layer = (int8_t)((uint8_t)(specialByte & MFConstants::_POI_LAYER_BITMASK) >> MFConstants::_POI_LAYER_SHIFT);
                // Bit 5-8 represent number of tag IDs
                int8_t numberOfTags = (int8_t)(specialByte & MFConstants::_POI_NUMBER_OF_TAGS_BITMASK);

                // get tags from ids (VBE-U)
                std::vector <Tag> decodedTags;
                if (!(readBuffer.readTags(poiTags, &decodedTags, numberOfTags))) {
                    return false;
                }

                int8_t featureByte = readBuffer.read_byte();

                // Bit 1-3 for optional features
                bool featureName = (featureByte & MFConstants::_POI_FEATURE_NAME) != 0;
                bool featureHouseNumber = (featureByte & MFConstants::_POI_FEATURE_HOUSE_NUMBER) != 0;
                bool featureElevation = (featureByte & MFConstants::_POI_FEATURE_ELEVATION) != 0;

                // check if POI has name
                if (featureName) {
                    Tag tag(MFConstants::_TAG_KEY_NAME, readBuffer.read_utf8());
                    decodedTags.push_back(tag);
                }

                // check if POI has house number
                if (featureHouseNumber) {
                    Tag tag(MFConstants::_TAG_KEY_HOUSE_NUMBER, readBuffer.read_utf8());
                    decodedTags.push_back(tag);
                }

                // check if POI has an elevation
                if (featureElevation) {
                    Tag tag(MFConstants::_TAG_KEY_ELEVATION, std::to_string(readBuffer.read_var_long()));
                    decodedTags.push_back(tag);
                }

                MapPos position(lon, lat);
                // depending on the zoom level configuration the poi can lie outside
                // the tile requested, we filter them out here
                bool contained = mapBounds.inside(position);
                if (!filterRequired || contained) {
                    pois->push_back(POI(layer, decodedTags, position));
                }
            }
            return true;
        }

        bool MapFile::processBlockSignature(ReadBuffer &readBuffer) {
            if (_map_file_header.getMapFileInfo()->getDebugInfo()) {
                std::string signatureBlock = readBuffer.read_utf8(MFConstants::_SIGNATURE_LENGTH_BLOCK);
                if (!signatureBlock.rfind("###TileStart", 0)) {
                    _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid block signature: %s", _tag, signatureBlock));
                    // Log::Warnf("MapFile::processBlockSignature: Invalid block signature: %s", signatureBlock);
                    return false;
                }
            }
            return true;
        }

        std::vector <std::vector<int32_t>>
        MapFile::readTileZoomTable(const SubFileParameters &subFileParams, ReadBuffer &readBuffer) {
            int32_t rows = subFileParams.getMaxZoomLevel() - subFileParams.getMinZoomLevel() + 1;
            std::vector <std::vector<int32_t>> zoomTable(rows, std::vector<int32_t>(2));

            int32_t numberPois = 0, numberWays = 0;

            // Loop over zoom levels inside a selected sub file
            for (int32_t row = 0; row < rows; ++row) {
                numberPois += readBuffer.read_var_ulong();
                numberWays += readBuffer.read_var_ulong();

                zoomTable[row][0] = numberPois;
                zoomTable[row][1] = numberWays;
            }

            return zoomTable;
        }

        uint64_t MapFile::readBlockIndex(const SubFileParameters &subFileParams, long blockNumber) {
            if (blockNumber >= subFileParams.getNumberOfBlocks()) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid block number: %d", _tag, blockNumber));
                // Log::Errorf("MapFile::readBlockIndex: Invalid block number: %d", blockNumber);
                //throw GenericException("Invalid block number");
                throw std::runtime_error(tfm::format("%s::Invalid block number", _tag));
            }

            // Calculate index block the query block falls in
            uint32_t indexBlockNumber = blockNumber / MFConstants::_INDEX_ENTRIES_PER_BLOCK;

            // Calculate postion of index block in sub file
            uint64_t indexBlockPosition = subFileParams.getIndexStartAddress() + indexBlockNumber * MFConstants::_SIZE_OF_INDEX_BLOCK;

            // Calculate remaining size of current index block
            uint64_t remainingIndexSize = subFileParams.getIndexEndAddress() - indexBlockPosition;

            // Calculate the size to read from the .map file in bytes to span the entire index block
            uint64_t indexBlockSize = std::min((uint64_t) MFConstants::_SIZE_OF_INDEX_BLOCK, (uint64_t) remainingIndexSize);

            _mutex.lock();
            ReadBuffer readBuffer(_filePath, _logger);
            // extract the data of the index block from the map file
            if (!readBuffer.readFromFile(indexBlockPosition, indexBlockSize)) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Could not read index block with size: %d", _tag, indexBlockSize));
                // Log::Errorf("MapFile::readBlockIndex: Could not read index block with size: %d", indexBlockSize);
                return -1;
            }
            _mutex.unlock();

            // calculate block entry of the actual block pointer entry inside the index block
            uint64_t indexEntryInIndexBlock = blockNumber % MFConstants::_INDEX_ENTRIES_PER_BLOCK;

            // alculate the position of the actual block pointer in the sub file
            uint32_t addressInIndexBlock = (uint32_t)(indexEntryInIndexBlock * MFConstants::_BYTES_PER_INDEX_ENTRY);

            readBuffer.set_buffer_position(addressInIndexBlock);
            // read the content of the index block to obtain the block position pointer
            uint64_t readIndexEntry = readBuffer.read_five_bytes_long();

            return readIndexEntry;
        }

        bool MapFile::processSingleBlock(const QueryParameters &queryParams, const SubFileParameters &subFileParams,
                                         const MapBounds2D &mapBounds, const MapPos &tileOrigin, Selector selector,
                                         TileDataBundle *bundle, ReadBuffer &readBuffer) {
            if (!processBlockSignature(readBuffer)) {
                return false;
            }

            auto zoomTable = readTileZoomTable(subFileParams, readBuffer);
            uint32_t zoomTableRow = queryParams.getQueryZoomLevel() - subFileParams.getMinZoomLevel();

            // consult the zoom table how many pois and ways to read for the current block based on the query zoom level
            uint32_t numPoisOnQueryZoomLevel = zoomTable[zoomTableRow][0];
            uint32_t numWaysOnQueryZoomLevel = zoomTable[zoomTableRow][1];

            // get the relative offset to the first stored way in block
            uint64_t firstWayOffset = readBuffer.read_var_ulong();
            if (firstWayOffset < 0) {
                _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid first way offset: %d", _tag, firstWayOffset));
                // Log::Warnf("MapFile::processBlock: Invalid first way offset: %d", firstWayOffset);
                return false;
            }

            firstWayOffset += readBuffer.getBufferPosition();
            if (firstWayOffset > readBuffer.getBufferSize()) {
                _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid first way offset: %d", _tag, firstWayOffset));
                // Log::Warnf("MapFile::processBlock: Invalid first way offset: %d", firstWayOffset);
                return false;
            }

            bool filterRequired = queryParams.getQueryZoomLevel() > subFileParams.getBaseZoomLevel();

            std::vector<POI> pois;
            pois.reserve(numPoisOnQueryZoomLevel);
            // read all pois for the current block
            if (!processPois(tileOrigin, numPoisOnQueryZoomLevel, mapBounds, filterRequired, &pois, readBuffer)) {
                return false;
            }

            std::vector <Way> ways;
            // only return pois and skip ways
            if (selector == Selector::POIS) {
                ways = std::vector<Way>();
            } else {
                if (readBuffer.getBufferPosition() > firstWayOffset) {
                    _logger->write(mvt::Logger::Severity::WARNING, tfm::format("%s::Invalid buffer position: %d", _tag, readBuffer.getBufferPosition()));
                    //Log::Warnf("MapFile::processSingleBlock: Invalid buffer position: %d", readBuffer.getBufferPosition());
                    return false;
                }

                // move read pointer to first way in the block
                readBuffer.set_buffer_position(firstWayOffset);
                ways.reserve(numWaysOnQueryZoomLevel);

                // read all ways of the current block
                if (!processWays(queryParams, numWaysOnQueryZoomLevel, mapBounds, filterRequired, tileOrigin, selector,
                                 &ways, readBuffer)) {
                    return false;
                }
            }

            bundle->setPois(pois);
            bundle->setWays(ways);

            return true;
        }
    }
}
