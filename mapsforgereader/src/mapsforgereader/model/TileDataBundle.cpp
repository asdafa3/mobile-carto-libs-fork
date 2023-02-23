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

#include "TileDataBundle.h"

namespace carto {
    TileDataBundle::TileDataBundle(std::vector<Way> ways, std::vector<POI> pois) {
        _ways = ways;
        _pois = pois;
    }

    TileDataBundle::TileDataBundle() {
    }

    void TileDataBundle::setPois(const std::vector<POI> &pois) {
        _pois = pois;
    }

    void TileDataBundle::setWays(const std::vector<Way> &ways) {
        _ways = ways;
    }

    std::vector<POI> TileDataBundle::getPOIs() const {
        return _pois;
    }

    std::vector<Way> TileDataBundle::getWays() const {
        return _ways;
    }
}