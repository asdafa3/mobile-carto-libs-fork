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

#include "OptionalFields.h"
#include <mapsforgereader/utils/LatLongUtils.h>

#include <mapsforgereader/MFConstants.h>
#include <mapsforgereader/header/MapFileInfoBuilder.h>

#include <stdexcept>

namespace carto {

    OptionalFields::OptionalFields(uint8_t flags, std::shared_ptr<mvt::Logger> logger) : _logger(std::move(logger)) {
        // check for existence of inficdual fields by bit-masking against the header flag
        _is_debug_file = (flags & MFConstants::_HEADER_BITMASK_DEBUG) != 0;
        _has_start_position = (flags & MFConstants::_HEADER_BITMASK_START_POSITION) != 0;
        _has_start_zoom_level = (flags & MFConstants::_HEADER_BITMASK_START_ZOOM_LEVEL) != 0;
        _has_language_preference = (flags & MFConstants::_HEADER_BITMASK_LANGUAGES_PREFERENCE) != 0;
        _has_comment = (flags & MFConstants::_HEADER_BITMASK_COMMENT) != 0;
        _has_created_by = (flags & MFConstants::_HEADER_BITMASK_CREATED_BY) != 0;
    }

    void OptionalFields::readOptionalFields(const std::shared_ptr<ReadBuffer> &readBuffer, MapFileInfoBuilder &mapFileInfoBuilder) {
        uint8_t flags = readBuffer -> read_byte();
        mapFileInfoBuilder._optionalFields = std::unique_ptr<OptionalFields>(new OptionalFields(flags, _logger));
        mapFileInfoBuilder._optionalFields -> readOptionalFields(readBuffer);
    }

    void OptionalFields::readOptionalFields(const std::shared_ptr<ReadBuffer> &readBuffer) {
        readMapStartPosition(readBuffer);
        readMapStartZoomLevel(readBuffer);
        readLanguagePreference(readBuffer);

        if (_has_comment) {
            _comment = readBuffer -> read_utf8();
        }

        if (_has_created_by) {
            _created_by = readBuffer -> read_utf8();
        }
    }

    void OptionalFields::readMapStartPosition(const std::shared_ptr<ReadBuffer> &readBuffer) {
        if (_has_start_position) {
            double mapStartLat = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
            double mapStartLon = LatLongUtils::microdegreesToDegrees(readBuffer -> read_int());
            MapPos startPos(mapStartLat, mapStartLon);
            _start_position = startPos;
        }
    }

    void OptionalFields::readMapStartZoomLevel(const std::shared_ptr<ReadBuffer> &readBuffer) {
        if (_has_start_zoom_level) {
            uint8_t mapStartZoomLevel = readBuffer -> read_byte();
            if (mapStartZoomLevel < 0 || mapStartZoomLevel > MFConstants::_START_ZOOM_LEVEL_MAX) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::invalid start zoom level: %d", _tag, mapStartZoomLevel));
                //Log::Errorf("OptionalFields::readMapStartZoomLevel: invalid start zoom level: %d", mapStartZoomLevel);
                //throw GenericException("OptionalFields::readMapStartZoomLevel: invalid start zoom level");
                throw std::runtime_error(tfm::format("%s::Invalid start zoom level", _tag));
            }

            _start_zoom_level = mapStartZoomLevel;
        }
    }

    void OptionalFields::readLanguagePreference(const std::shared_ptr<ReadBuffer> &readBuffer) {
        if (_has_language_preference) {
            _lang_preference = readBuffer -> read_utf8();
        }
    }

    const std::string & OptionalFields::getComment() const {
        return _comment;
    }

    const std::string & OptionalFields::getCreatedBy() const {
        return _created_by;
    }

    const std::string & OptionalFields::getLanguagePreference() const {
        return _lang_preference;
    }

    const mvt::MapPos & OptionalFields::getStartPosition() const {
        return _start_position;
    }

    const uint8_t & OptionalFields::getStartZoomLevel() const {
        return _start_zoom_level;
    }

    const bool & OptionalFields::getIsDebugFile() const {
        return _is_debug_file;
    }
}