//
// Created by Linus Schwarz on 20.07.21.
//

#ifndef MOBILE_SDK_TYPES_H
#define MOBILE_SDK_TYPES_H

#include <cglib/vec.h>
#include <cglib/bbox.h>
#include <vt/TileId.h>
#include <vector>
#include <variant>
#include <type_traits>

namespace carto {
    namespace mvt {
        typedef cglib::vec3<double> MapPos;
        typedef cglib::bbox3<double> MapBounds;
        typedef vt::TileId MapTile;
    }
}

#endif //MOBILE_SDK_TYPES_H
