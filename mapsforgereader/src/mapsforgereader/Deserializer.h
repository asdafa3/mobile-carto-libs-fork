/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
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

#ifndef CARTO_MOBILE_SDK_DESERIALIZER_H
#define CARTO_MOBILE_SDK_DESERIALIZER_H

#include <iostream>
#include <vector>

namespace carto {
    /**
     * An utility class to convert byte vectors to numbers.
     */
    class Deserializer {
    public:

        /**
         * Converts five bytes of a byte array to an unsigned long.
         * The byte order is big-endian.
         *
         * @param data Buffer of bytes to read from.
         * @param offset Offset in buffer.
         * @return 4 Byte signed integer.
         */
        static int32_t getInt(const std::vector<char> &data, int offset);

        /**
         * Converts eight bytes of a byte array to a signed long.
         * The byte order is big-endian.
         *
         * @param data Buffer of bytes to read from.
         * @param offset Offset in buffer.
         * @return 8 Byte unsigned integer.
         */
        static uint64_t getFiveBytesLong(const std::vector<char> &data, int offset);

        /**
         * Converts eight bytes of a byte array to a signed long.
         * The byte order is big-endian.
         *
         * @param data Buffer of bytes to read from.
         * @param offset Offset in buffer.
         * @return 8 Byte unsigned integer.
         */
        static int64_t getLong(const std::vector<char> &data, int offset);

        /**
         * Converts two bytes of a byte array to a signed int.
         * The byte order is big-endian.
         *
         * @param data Buffer of bytes to read from.
         * @param offset Offset in buffer.
         * @return 2 Byte unsigned integer.
         */
        static int16_t getShort(const std::vector<char> &data, int offset);

    private:
        Deserializer();
    };
}


#endif //CARTO_MOBILE_SDK_DESERIALIZER_H
