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

#include "MapQueryResult.h"

#include <algorithm>

namespace carto {
    MapQueryResult::MapQueryResult() { }

    void MapQueryResult::add(const TileDataBundle &bundle) {
        // add all ways
        for (auto const &way : bundle.getWays()) {
            _ways.push_back(way);
        }
        // add all pois
        for (auto const &poi : bundle.getPOIs()) {
            _pois.push_back(poi);
        }
    }

    void MapQueryResult::merge(const MapQueryResult &other, bool checkDuplicates) {
        if (checkDuplicates) {
            for (auto const &poi : other.getPOIs()) {
                // check if poi is not already contained
                if (std::find(_pois.begin(), _pois.end(), poi) == _pois.end()) {
                    _pois.push_back(poi);
                }
            }
            for (auto const &way : other.getWays()) {
                // check if way is not already contained
                if (std::find(_ways.begin(), _ways.end(), way) == _ways.end()) {
                    _ways.push_back(way);
                }
            }
        } else {
            for (auto const &poi : other.getPOIs()) {
                _pois.push_back(poi);
            }
            for (auto const &way : other.getWays()) {
                _ways.push_back(way);
            }
        }
    }

    std::vector<Way> MapQueryResult::getWays() const {
        return _ways;
    }

    std::vector<POI> MapQueryResult::getPOIs() const {
        return _pois;
    }
}