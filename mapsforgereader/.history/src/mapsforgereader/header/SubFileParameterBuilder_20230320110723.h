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

#ifndef CARTO_MOBILE_SDK_SUBFILEPARAMETERBUILDER_H
#define CARTO_MOBILE_SDK_SUBFILEPARAMETERBUILDER_H

#include <cglib/bbox.h>

namespace carto {

    class SubFileParameters;

    /**
     * Builds SubFileParameters objects.
     */
    class SubFileParameterBuilder {
    public:
        typedef cglib::bbox2<double> MapBounds;

        SubFileParameterBuilder();

        SubFileParameters build() const;

        uint8_t _base_zoom_level;
        MapBounds _bounding_box;
        uint64_t _index_start_address;
        uint64_t _start_address;
        uint8_t _zoom_level_max;
        uint8_t _zoom_level_min;
        uint64_t _sub_file_size;
    };
}


#endif //CARTO_MOBILE_SDK_SUBFILEPARAMETERBUILDER_H
