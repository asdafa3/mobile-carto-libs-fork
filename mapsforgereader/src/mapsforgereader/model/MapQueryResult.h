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

#ifndef CARTO_MOBILE_SDK_MAPQUERYRESULT_H
#define CARTO_MOBILE_SDK_MAPQUERYRESULT_H

#include <mapsforgereader/model/POI.h>
#include <mapsforgereader/model/Way.h>
#include <mapsforgereader/model/TileDataBundle.h>

#include <vector>

namespace carto {
    /**
     * Container for map query results. Can contain pois and ways for one
     * tile on the map and provides functionality to merge different bundles.
     */
    class MapQueryResult {
    public:
        MapQueryResult();

        /**
         * Add a poi or way data bundle of one block in the map file to the result
         * bundle. This is done as a query tile can span multiple blocks.
         * @param bundle
         */
        void add(const TileDataBundle &bundle);

        /**
         * Merge data bundles. E.g. when the same tile on different .map files
         * is requested.
         *
         * @param other The poi and way data bundle to merge in.
         * @param checkDuplicates True if dublicates should be removed.
         */
        void merge(const MapQueryResult &other, bool checkDuplicates);

        std::vector<POI> getPOIs() const;

        std::vector<Way> getWays() const;
    private:
        /**
         * All ways of a tile.
         */
        std::vector<Way> _ways;

        /**
         * All pois of a tile.
         */
        std::vector<POI> _pois;
    };
}


#endif //CARTO_MOBILE_SDK_MAPQUERYRESULT_H
