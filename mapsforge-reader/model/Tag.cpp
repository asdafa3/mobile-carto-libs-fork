/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
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

#include "Tag.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "mapsforge-reader/MFConstants.h"

namespace carto {
    Tag::Tag(std::string tag)
    : Tag(tag, tag.find(MFConstants::_KEY_VALUE_SEPARATOR))
    { }

    Tag::Tag(std::string key, std::string value) {
        _key = key;
        _value = value;
    }

    Tag::Tag(std::string tag, int split_position)
    : Tag(tag.substr(0, split_position), tag.substr(split_position + 1))
    { }

    std::string Tag::getKey() const {
        return _key;
    }

    std::string Tag::getValue() const {
        return _value;
    }

    bool Tag::operator==(const Tag &tag) const {
        return _key == tag._key && _value == tag._value;
    }

    bool Tag::operator!=(const Tag &tag) const {
        return !(*this == tag);
    }

    int Tag::hash() const {
        return -1;
    }

    std::string Tag::toString() const {
        std::stringstream ss;
        ss << std::setiosflags(std::ios::fixed);
        ss << "Tag [key=" << _key << ", value=" << _value << "]";
        return ss.str();
    }
}