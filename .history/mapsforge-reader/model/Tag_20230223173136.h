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

#ifndef CARTO_MOBILE_SDK_TAG_H
#define CARTO_MOBILE_SDK_TAG_H

#include <string>
#include "Tag.h"

namespace carto {
    /**
     * Data model for creating storing a feature tag in form of a key value pair.
     */
    class Tag {
    public:
        Tag(std::string key, std::string value);

        Tag(std::string tag);

        std::string getKey() const;

        std::string getValue() const;

        bool operator ==(const Tag &tag) const;

        bool operator !=(const Tag &tag) const;

        int hash() const;

        std::string toString() const;
    private:
        Tag(std::string tag, int split_position);
    private:
        /**
         * Key of a tag.
         */
        std::string _key;

        /**
         * Value of tag.
         */
        std::string _value;
    };
}


#endif //CARTO_MOBILE_SDK_TAG_H
