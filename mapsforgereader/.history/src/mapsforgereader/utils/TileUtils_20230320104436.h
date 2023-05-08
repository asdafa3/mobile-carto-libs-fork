#ifndef CARTO_MOBILE_SDK_PROJECTIONUTILS_H
#define CARTO_MOBILE_SDK_PROJECTIONUTILS_H

#include <cglib/vec.h>
#include <vt/TileId.h>

namespace carto {
    class TileUtils {
        public: 
            typedef cglib::vec2<double> MapPos;
            typedef vt::TileId MapTile;
            
            virtual ~TileUtils();

            virtual MapTile CalculateMapTile(const MapPos& pos) const;
        protected:
            explicit TileUtils();
    };
}

#endif