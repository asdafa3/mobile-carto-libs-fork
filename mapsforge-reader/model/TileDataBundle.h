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

#ifndef CARTO_MOBILE_SDK_TILEDATABUNDLE_H
#define CARTO_MOBILE_SDK_TILEDATABUNDLE_H

#include "mapsforge-reader/model/Way.h"
#include "mapsforge-reader/model/POI.h"

#include <vector>

namespace carto {
    /**
     * Way and poi data bundle for one block in a map file.
     */
    class TileDataBundle {
    public:
        TileDataBundle(std::vector<Way> ways, std::vector<POI> pois);
        TileDataBundle();

        void setWays(const std::vector<Way> &ways);
        std::vector<Way> getWays() const;

        void setPois(const std::vector<POI> &pois);
        std::vector<POI> getPOIs() const;
    private:
        /**
         * All ways of a block.
         */
        std::vector<Way> _ways;

        /**
         * All pois of a block
         */
        std::vector<POI> _pois;
    };
}


#endif //CARTO_MOBILE_SDK_TILEDATABUNDLE_H
