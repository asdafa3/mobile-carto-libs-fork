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

#include "Way.h"

namespace carto {
    Way::Way(int8_t layer, const std::vector<Tag>& tags, const std::vector<std::vector<MapPos>>& coords, MapPos labelPosition) {
        _layer = layer;
        _tags = tags;
        _coords = coords;
        _label_position = labelPosition;
    }

    mvt::MapPos Way::getLabelPosition() const {
        return _label_position;
    }

    std::vector<std::vector<mvt::MapPos>> Way::getCoords() const {
        return _coords;
    }

    uint8_t Way::getLayer() const {
        return _layer;
    }

    std::vector<Tag> Way::getTags() const {
        return _tags;
    }

    bool Way::operator==(const Way &way) const {

        if (_tags.size() != way.getTags().size()) {
            return false;
        }
        for (int i = 0; i < _tags.size(); ++i) {
            // Assumption: Tags are ordered
            if (_tags[i] != way.getTags()[i]) {
                return false;
            }
        }

        if (_layer != way.getLayer()) {
            return false;
        }

        return true;
    }

    bool Way::operator!=(const Way &way) const {
        return !(*this == way);
    }

    int Way::hash() {
        return -1;
    }

    std::string Way::toString() {
        return "";
    }
}