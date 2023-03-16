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

#include "ReadBuffer.h"
#include "Deserializer.h"
#include <mapsforgereader/MFConstants.h>

#include <iostream>
#include <stdexcept>
#include <fstream>

namespace carto {
     
    ReadBuffer::ReadBuffer(const std::string &path_to_map_file, std::shared_ptr<mvt::Logger> logger) :
    _data(),
    _tag_ids(),
    _stream(path_to_map_file.c_str(), std::ios::in | std::ios::binary),
    _logger(std::move(logger))
    {
        if(_stream.fail()) {
            _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Error opening map file at path '%s'", _tag, path_to_map_file));
            // Log::Errorf("ReadBuffer::Error opening map file at path '%s'", path_to_map_file);
            throw std::runtime_error(tfm::format("%s::Error opening map file.", _tag));
        }
        long begin, end;
        begin = _stream.tellg();
        _stream.seekg(0, std::ios::end);
        end = _stream.tellg();
        _file_size = end-begin;
    }

    ReadBuffer::~ReadBuffer() {
        _stream.close();
    }

    bool ReadBuffer::readFromFile(int length) {
        std::vector<char> bytes(length);
        
        _stream.seekg(0);

        if (!_stream.read((char*) &bytes[0], length)) {
            _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Error reading from map file", _tag));
            // Log::Error("ReadBuffer::Error reading from map file");
            return false;
        }

        _data = bytes;
        _buffer_position = 0;
        _bufferSize = _data.size();
        return true;
    }

    bool ReadBuffer::readFromFile(int offset, int length) {
        std::vector<char> bytes(length);

        _stream.seekg(offset);

        if (!_stream.read((char*) &bytes[0], length)) {
            _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Error reading from map file", _tag));
            // Log::Error("ReadBuffer::Error reading from map file");
            return false;
        }

        _data = bytes;
        _buffer_position = 0;
        _bufferSize = _data.size();
        return true;
    }

    uint64_t ReadBuffer::getBufferSize() {
        return _bufferSize;
    }

    void ReadBuffer::reset_buffer_position() {
        _buffer_position = 0;
    }

    void ReadBuffer::set_buffer_position(int offset) {
        if (offset <= _data.size()) {
            _buffer_position = offset;
        }
    }

    void ReadBuffer::skip_bytes(int num_bytes) {
        if (_buffer_position + num_bytes <= _data.size()) {
            _buffer_position += num_bytes;
        }
    }

    int8_t ReadBuffer::read_byte() {
        return (int8_t)_data[_buffer_position++];
    }

    int16_t ReadBuffer::read_short() {
        _buffer_position += 2;
        return Deserializer::getShort(_data, _buffer_position - 2);
    }

    int32_t ReadBuffer::read_int() {
        _buffer_position += 4;
        return Deserializer::getInt(_data, _buffer_position - 4);
    }

    int64_t ReadBuffer::read_long() {
        _buffer_position += 8;
        return Deserializer::getLong(_data, _buffer_position - 8);
    }

    uint64_t ReadBuffer::read_five_bytes_long() {
        _buffer_position += 5;
        return Deserializer::getFiveBytesLong(_data, _buffer_position - 5);
    }

    int64_t ReadBuffer::read_var_long() {
        int variable_byte_decode = 0;
        char variable_byte_shift = 0;

        // check if first bit (continuation bit is set)
        while (_data[_buffer_position] & 0x80) {
            variable_byte_decode |= (_data[_buffer_position++] & 0x7f) << variable_byte_shift;

            // if first bit is not set in current byte, next byte contains 7 bits of data
            variable_byte_shift += 7;
        }

        // read six bits of data from last byte
        if (_data[_buffer_position] & 0x40) {
            // value is negative
            return -(variable_byte_decode | ((_data[_buffer_position++] & 0x3f) << variable_byte_shift));
        }

        // positive
        return variable_byte_decode | ((_data[_buffer_position++] & 0x3f) << variable_byte_shift);
    }

    uint64_t ReadBuffer::read_var_ulong() {
        int variable_byte_decode = 0;
        char variable_byte_shift = 0;

        // check if first bit (continuation bit is set)
        while (_data[_buffer_position] & 0x80) {
            variable_byte_decode |= (_data[_buffer_position++] & 0x7f) << variable_byte_shift;

            // if first bit is not set in current byte, next byte contains 7 bits of data
            variable_byte_shift += 7;
        }

        // read the seven data bits from the last byte
        return variable_byte_decode | (_data[_buffer_position++] << variable_byte_shift);
    }

    float ReadBuffer::read_float() {
        int64_t val = read_int();
        return *reinterpret_cast<float *>(&val);
    }

    std::string ReadBuffer::read_utf8() {
        int num_characters = read_var_ulong();
        return read_utf8(num_characters);
    }

    std::string ReadBuffer::read_utf8(int n) {
        if (n > 0 && _buffer_position + n <= _data.size()) {
            _buffer_position += n;
            std::string str(_data.begin() + (_buffer_position - n), _data.begin() + _buffer_position);
            return str;
        }
        return "";
    }

     bool ReadBuffer::readTags(const std::vector<Tag> &tagsIn, std::vector<Tag> *tagsOut, int8_t numberOfTags) {
        std::vector<Tag> tags;
        _tag_ids.clear();

        uint32_t maxTag = tagsIn.size();

        for (uint8_t tagIndex = numberOfTags; tagIndex != 0; --tagIndex) {
            uint64_t tagId = read_var_ulong();
            if (tagId < 0 || tagId >= maxTag) {
                _logger->write(mvt::Logger::Severity::ERROR, tfm::format("%s::Invalid tag ID: %d", _tag, tagId));
                // Log::Warnf("ReadBuffer::readTags: Invalid tag ID: %d", tagId);
                return false;
            }
            _tag_ids.push_back(tagId);
        }

         for (auto &tagId : _tag_ids) {
             Tag tag = tagsIn[tagId];
             // decode values of tags
             if (tag.getValue().size() == 2 && tag.getValue()[0] == '%') {
                 std::string value = tag.getValue();
                 if (value[1] == 'b') {
                     value = std::to_string(read_byte());
                 } else if (value[1] == 'i') {
                     if (tag.getKey().find(":colour") != std::string::npos) {
                         value = "#" + integralToHexstr(read_int());
                     } else {
                         value = std::to_string(read_int());
                     }
                 } else if (value[1] == 'f') {
                     value = std::to_string(read_float());
                 } else if (value[1] == 'h') {
                     value = std::to_string(read_short());
                 } else if (value[1] == 's') {
                     value = read_utf8();
                 }
                 tag = Tag(tag.getKey(), value);
             }
             tagsOut->push_back(tag);
         }

         return true;
    }

    const uint64_t & ReadBuffer::getCurrentFileSize() const {
        return _file_size;
    }

    uint64_t ReadBuffer::getBufferPosition() {
        return _buffer_position;
    }
}
