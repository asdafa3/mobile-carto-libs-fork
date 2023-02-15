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

#ifndef CARTO_MOBILE_SDK_READBUFFER_H
#define CARTO_MOBILE_SDK_READBUFFER_H

#include "mapsforge-reader/model/Tag.h"

#include <cstddef>
#include <string>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <vector>
#include <tinyformat.h>

#include "mapnikvt/Logger.h"

namespace carto {

    /*
     * Reads a binary file into a buffer and decodes data.
     */
    class ReadBuffer {
    public:

        using Logger = carto::mvt::Logger;
    
        ReadBuffer(const std::string &path_to_map_file, std::shared_ptr<Logger> logger);
        virtual ~ReadBuffer();

        /*
         * Reads a byte from the buffer at the current buffer position.
         * @return The bytes that was read.
         */
        int8_t read_byte();

        /*
         * Converts two bytes from the read buffer to a signed short.
         * The byte order is big-endian.
         * @return Integer value
         */
        int16_t read_short();

        /*
         * Converts four bytes from the read buffer to a signed int.
         * The byte order is big-endian.
         * @return Integer value
         */
        int32_t read_int();

        /*
         * Converts 8 bytes from the read buffer to a signed long.
         * The byte order is big-endian.
         * @return Integer value
         */
        int64_t read_long();

        /**
         * Converts a variable amount of bytes from the read buffer to a signed int.
         * The first bit is for continuation info, the other six (last byte) or seven (all other bytes) bits are for data.
         * The second bit in the last byte indicates the sign of the number.
         *
         * @return Int value.
         */
        int64_t read_var_long();

        /**
         * Converts a variable amount of bytes from the read buffer to an unsigned int.
         * The first bit is for continuation info, the other seven bits are for data.
         *
         * @return Int value.
         */
        uint64_t read_var_ulong();

        /**
         * Reads 5 byte from buffer and converts it to 8 byte unsigned long.
         *
         * @return Int value.
         */
        uint64_t read_five_bytes_long();

        /**
         * Converts four bytes from the read buffer to a float.
         * The byte order is big-endian.
         *
         * @return the float value.
         */
        float read_float();

        /**
         * Reads an utf8 string from file.
         *
         * @return utf8 String
         */
        std::string read_utf8();

        /**
         * Reads an utf8 string from file of specified length.
         * @param Numbber of characters.
         * @return utf8 string.
         */
        std::string read_utf8(int n);

        /**
         * Decodes tags by resolving integer references to tags stored in .map file header.
         * @param tagsIn Input vector of tags.
         * @param tagsOut Decoded tags.
         * @param numberOfTags Number of tags for a way or poi.
         * @return True if tags could be decoded.
         */
        bool readTags(const std::vector<Tag> &tagsIn, std::vector<Tag> *tagsOut, int8_t numberOfTags);

        /**
         * Reads the given amount of bytes from the file into the read buffer and resets the internal buffer position.
         *
         * @param length the amount of bytes to read from the file.
         * @return true if the whole data was read successfully, false otherwise.
         */
        bool readFromFile(int length);

        /**
         * Reads the given amount of bytes from the file, starting at an offset position, into the read buffer and resets the internal buffer position.
         * @param offset the offset position, measured in bytes from the beginning of the file, at which to set the file pointer.
         * @param length the amount of bytes to read from the file.
         * @return true if the whole data was read successfully, false otherwise.
         */
        bool readFromFile(int offset, int length);

        /**
         * Returns the current buffer position.
         * @return Current buffer position.
         */
        uint64_t getBufferPosition();

        /**
         * Get the amount of bytes in the buffer. Dependant on how many bytes have previously been read.
         * @return Amount of bytes in the buffer.
         */
        uint64_t getBufferSize();

        /**
         * The size of the file wrapped by the
         * @return
         */
        const uint64_t &getCurrentFileSize() const;

        /**
         * Fast forwards the buffer position by n bytes.
         *
         * @param bytes Bytes to skip.
         */
        void skip_bytes(int bytes);

        /**
         * Reset the buffer position to zero.
         */
        void reset_buffer_position();

        /**
         * Set the buffer position to an arbitrary value.
         * @param offset
         */
        void set_buffer_position(int offset);
    private:
        /**
         * Input stream used for reading from .map file.
         */
        std::ifstream _stream;

        /**
         * Tag reference integers.
         */
        std::vector<uint32_t> _tag_ids;

        /**
         * Path of file to read from.
         */
        std::string _filePath;

        /**
         * Internal buffer position.
         */
        uint64_t _buffer_position;

        /**
         * Size of the file to read from.
         */
        uint64_t _file_size;

        /**
         * Size in buffer after reading from file.
         */
        uint64_t _bufferSize;

        /**
         * The most recent read data.
         */
        std::vector<char> _data;

        const std::string _tag = "ReadBuffer";

        const std::shared_ptr<Logger> _logger;
    };
}

namespace carto {
    /**
     * Utility function to create hexadecimal string from an integer.
     */
    template <typename I>
    std::string integralToHexstr(I w, size_t hex_len = sizeof(I)<<1) {
        static const char* digits = "0123456789ABCDEF";
        std::string rc(hex_len,'0');
        for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
            rc[i] = digits[(w>>j) & 0x0f];
        return rc;
    }
}


#endif //CARTO_MOBILE_SDK_READBUFFER_H
