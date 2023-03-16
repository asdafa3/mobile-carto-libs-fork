/*
 * Copyright 2010, 2011, 2012, 2013 mapsforge.org
 * Copyright 2014-2015 Ludwig M Brinckmann
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
#ifndef CARTO_MOBILE_SDK_WAY_H
#define CARTO_MOBILE_SDK_WAY_H

#include <mapsforgereader/model/Tag.h>
#include <mapnikvt/Types.h>>
#include <vector>

namespace carto {
    /**
     * Data model for an Mapsforge way.
     * Can contain multiple segments and each segment can contain multiple coordinate blocks.
     */
    class Way {
    public:
        

        Way(int8_t layer, const std::vector<Tag>& tags, const std::vector<std::vector<MapPos>>& coords, MapPos labelPosition);

        MapPos getLabelPosition() const;

        std::vector<std::vector<MapPos>> getCoords() const;

        uint8_t getLayer() const;

        std::vector<Tag> getTags() const;

        bool operator ==(const Way &way) const;

        bool operator !=(const Way &way) const;

        static int hash() ;

        static std::string toString() ;
    private:
        /**
         * Position of a label in lat lon degrees.
         */
        MapPos _label_position;

        /**
         * All coordinates of a way.
         */
        std::vector<std::vector<MapPos>> _coords;

        /**
         * Osm "layer=" tag value.
         */
        uint8_t _layer;

        /**
         * Tags for the current way.
         */
        std::vector<Tag> _tags;
    };
}

namespace std {

    template<>
    struct hash<carto::Way> {

    };
}

#endif //CARTO_MOBILE_SDK_WAY_H
