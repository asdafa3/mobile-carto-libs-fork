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

#ifndef CARTO_MOBILE_SDK_POI_H
#define CARTO_MOBILE_SDK_POI_H

#include <mapsforgereader/model/Tag.h>
#include <cglib/vec.h>
#include <vector>


namespace carto {
    /**
     * Data model for a point of interest on the map.
     */
    class POI {
    public:
        typedef cglib::vec3<double> MapPos;

        POI(int8_t layer, std::vector<Tag> tags, MapPos position);

        int8_t getLayer() const;

        MapPos getPosition() const;

        std::vector<Tag> getTags() const;

        bool operator ==(const POI &poi) const;

        bool operator !=(const POI &poi) const;

        int hash() const;

        std::string toString() const;
    private:
        /**
         * OSM "layer=" tag value.
         */
        int8_t _layer;

        /**
         * Position of of the POI in lat long coordinates.
         */
        MapPos _position;

        /**
         * The tags of this POI.
         */
        std::vector<Tag> _tags;
    };
}


#endif //CARTO_MOBILE_SDK_POI_H
