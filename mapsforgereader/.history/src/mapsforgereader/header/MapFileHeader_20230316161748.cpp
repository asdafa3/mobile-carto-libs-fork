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

#include "MapFileHeader.h"
#include "RequiredFields.h"
#include "OptionalFields.h"

#include <mapsforgereader/MFConstants.h>

#include <stdexcept>
#include <climits>

namespace carto {

    MapFileHeader::MapFileHeader(std::shared_ptr<mvt::Logger> logger) : _logger(std::move(logger)) { }

    MapFileHeader::~MapFileHeader() { }

    uint8_t MapFileHeader::getQueryZoomLevel(uint8_t zoom_level) {
        if (zoom_level > _map_file_info->getMaxZoomLevel()) {
            return _map_file_info->getMaxZoomLevel();
        } else if (zoom_level < _map_file_info->getMinZoomLevel()) {
            return _map_file_info->getMinZoomLevel();
        }
        return zoom_level;
    }

    SubFileParameters MapFileHeader::getSubFileParameters(int query_zoom_level) {
        return _sub_file_parameters[query_zoom_level];
    }

    void MapFileHeader::readHeader(const std::shared_ptr<ReadBuffer> &readBuffer, uint64_t file_size) {

        RequiredFields::readMagicByte(readBuffer);
        RequiredFields::readRemainingHeader(readBuffer);

        // read all meta data (required and optional) in the order they are stored in the .map file header

        MapFileInfoBuilder builder {};
        RequiredFields::readFileVersion(readBuffer, builder);
        RequiredFields::readFileSize(readBuffer, file_size, builder);
        RequiredFields::readMapDate(readBuffer, builder);
        RequiredFields::readBoundingBox(readBuffer, builder);
        RequiredFields::readTilePixelSize(readBuffer, builder);
        RequiredFields::readProjectionName(readBuffer, builder);
        OptionalFields::readOptionalFields(readBuffer, builder);
        RequiredFields::readPoiTags(readBuffer, builder);
        RequiredFields::readWayTags(readBuffer, builder);


        // read the sub file configuration stored int the sub file
        readSubFileParameters(readBuffer, file_size, builder);

        // construct map file info object
        _map_file_info = std::make_shared<MapFileInfo>(builder.build());

        std::cout << _map_file_info->toString() << std::endl;
    }

    void MapFileHeader::readSubFileParameters(const std::shared_ptr<ReadBuffer> &read_buffer, uint64_t file_size, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint8_t numberOfSubFiles = read_buffer -> read_byte();
        if (numberOfSubFiles < 1) {
            _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid number of sub files: {}", _tag, numberOfSubFiles));
            //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid number of sub files: %d", numberOfSubFiles);
            //throw GenericException("MapFileHeader::readSubFileParameters: Invalid number of sub files");
            throw std::runtime_error(tfm::format("%s::Invalid number of sub files!", _tag));
        }
        mapFileInfoBuilder._number_of_sub_files = numberOfSubFiles;

        uint8_t minZoomLevel = UINT8_MAX;
        uint8_t maxZoomLevel = 0;

        std::vector<SubFileParameters> tempSubFileParameters;
        tempSubFileParameters.resize(numberOfSubFiles);

        for (uint8_t currentSubFile = 0; currentSubFile < numberOfSubFiles; ++currentSubFile) {
            SubFileParameterBuilder builder {};

            // read base zoom level
            uint8_t baseZoomLevel = read_buffer -> read_byte();

            if (baseZoomLevel < 0 || baseZoomLevel > MFConstants::_BASE_ZOOM_LEVEL_MAX) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid base zoom level: %d", _tag, baseZoomLevel));
                // Log::Errorf("MapFileHeader::readSubFileParameters: Invalid base zoom level: %d", baseZoomLevel);
                // throw GenericException("MapFileHeader::readSubFileParameters: Invalid base zoom level");
                throw std::runtime_error(tfm::format("%s::Invalid base zoom level", _tag));
            }
            builder._base_zoom_level = baseZoomLevel;

            // read min zoom level
            uint8_t zoomLevelMin = read_buffer -> read_byte();
            if (zoomLevelMin < 0 || zoomLevelMin > 22) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid min zoom level: %d", _tag, zoomLevelMin));
                //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid min zoom level: %d", zoomLevelMin);
                //throw GenericException("MapFileHeader::readSubFileParameters: Invalid min zoom level");
                throw std::runtime_error(tfm::format("%s::Invalid min zoom level", _tag));
            }
            builder._zoom_level_min = zoomLevelMin;

            // read max zoom level
            uint8_t zoomLevelMax = read_buffer -> read_byte();
            if (zoomLevelMax < 0 || zoomLevelMax > 22) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid max zoom level: %d", _tag, zoomLevelMax));
                //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid max zoom level: %d", zoomLevelMax);
                //throw GenericException("MapFileHeader::readSubFileParameters: Invalid max zoom level");
                throw std::runtime_error(tfm::format("%s::Invalid max zoom level", _tag));
            }
            builder._zoom_level_max = zoomLevelMax;

            if (zoomLevelMin > zoomLevelMax) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid zoom range: %d to %d", _tag, zoomLevelMin, zoomLevelMax));
                //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid zoom range: %d to %d", zoomLevelMax);
                //throw GenericException("MapFileHeader::readSubFileParameters: Invalid zoom range");
                throw std::runtime_error(tfm::format("%s::Invalid zoom range", _tag));
            }


            // read start address of sub file
            uint64_t startAddress = read_buffer -> read_long();
            if (startAddress < MFConstants::_HEADER_SIZE_MIN || startAddress >= file_size) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid start address: %d", _tag, startAddress));
                //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid start address: %d", startAddress);
                //throw GenericException("MapFileHeader::readSubFileParameters: Invalid start address");
                throw std::runtime_error(tfm::format("%s::Invalid start address", _tag));
            }
            builder._start_address = startAddress;

            uint64_t indexStartAddress = startAddress;

            // if sub file contains debug signature, add the size of the debug signature to get the start position of
            // a sub files index block segment.
            if (mapFileInfoBuilder._optionalFields->getIsDebugFile()) {
                indexStartAddress += MFConstants::_SIGNATURE_LENGTH_INDEX;
            }
            builder._index_start_address = indexStartAddress;

            uint64_t subFileSize = read_buffer -> read_long();
            if (subFileSize < 1) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid sub file size: %d", _tag, subFileSize));
                //Log::Errorf("MapFileHeader::readSubFileParameters: Invalid sub file size: %d", subFileSize);
                //throw GenericException("MapFileHeader::readSubFileParameters:  Invalid sub file size");
                throw std::runtime_error(tfm::format("%s::Invalid subfile size", _tag));
            }
            builder._sub_file_size = subFileSize;

            builder._bounding_box = mapFileInfoBuilder._bounding_box;

            tempSubFileParameters[currentSubFile] = builder.build();

            if (minZoomLevel > tempSubFileParameters[currentSubFile].getMinZoomLevel()) {
                minZoomLevel = tempSubFileParameters[currentSubFile].getMinZoomLevel();
                mapFileInfoBuilder._zoom_level_min = minZoomLevel;
            }
            if (maxZoomLevel < tempSubFileParameters[currentSubFile].getMaxZoomLevel()) {
                maxZoomLevel = tempSubFileParameters[currentSubFile].getMaxZoomLevel();
                mapFileInfoBuilder._zoom_level_max = maxZoomLevel;
            }
        }

        // assign each zoom level to the sub file it belongs to by comparing it against min and max zoom level of the zoom intervals
        _sub_file_parameters.resize(maxZoomLevel + 1);
        for (int currentMapFile = 0; currentMapFile < numberOfSubFiles; ++currentMapFile) {
            SubFileParameters sfp = tempSubFileParameters[currentMapFile];
            for (uint8_t zoomLevel = sfp.getMinZoomLevel(); zoomLevel <= sfp.getMaxZoomLevel(); ++zoomLevel) {
                _sub_file_parameters[zoomLevel] = sfp;
            }
        }
    }

    const std::shared_ptr<MapFileInfo> & MapFileHeader::getMapFileInfo() const {
        return _map_file_info;
    }
}