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

#include "POI.h"

namespace carto {
    POI::POI(int8_t layer, std::vector<Tag> tags, MapPos position) {
        _layer = layer;
        _tags = tags;
        _position = position;
    }

    int8_t POI::getLayer() const {
        return _layer;
    }

    mvt::MapPos POI::getPosition() const {
        return _position;
    }

    std::vector<Tag> POI::getTags() const {
        return _tags;
    }

    bool POI::operator==(const POI &poi) const {
        return true;
    }

    bool POI::operator!=(const POI &poi) const {
        return !(*this == poi);
    }

    int POI::hash() const {
        return -1;
    }

    std::string POI::toString() const {
        return "";
    }
}