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

#include "mapsforge-reader/Deserializer.h"

namespace carto {
    int32_t Deserializer::getInt(const std::vector<char> &data, int offset) {
        return (int32_t)(data[offset] << 24
            | (data[offset + 1] & 0xff) << 16
            | (data[offset + 2] & 0xff) << 8
            | (data[offset + 3] & 0xff));
    }

    uint64_t Deserializer::getFiveBytesLong(const std::vector<char> &data, int offset) {
        return (data[offset] & 0xffL) << 32
            | (data[offset + 1] & 0xffL) << 24
            | (data[offset + 2] & 0xffL) << 16
            | (data[offset + 3] & 0xffL) << 8
            | (data[offset + 4] & 0xffL);
    }

    int64_t Deserializer::getLong(const std::vector<char> &data, int offset) {
        return (int64_t)(data[offset] & 0xffL) << 56
            | (data[offset + 1] & 0xffL) << 48
            | (data[offset + 2] & 0xffL) << 40
            | (data[offset + 3] & 0xffL) << 32
            | (data[offset + 4] & 0xffL) << 24
            | (data[offset + 5] & 0xffL) << 16
            | (data[offset + 6] & 0xffL) << 8
            | (data[offset + 7] & 0xffL);
    }

    int16_t Deserializer::getShort(const std::vector<char> &data, int offset) {
        return (int16_t)(data[offset] << 8 | (data[offset + 1] & 0xff));
    }

    Deserializer::Deserializer() {
    }
}