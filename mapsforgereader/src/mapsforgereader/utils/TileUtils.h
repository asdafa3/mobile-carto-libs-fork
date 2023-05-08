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

            virtual MapTile CalculateMapTile(const MapPos& pos, int zoom) const;
        protected:
            explicit TileUtils();

            // FIXME
            const int PI = 3.1415926535897932;
            const int EARTH_RADIUS = 6378137.0;
    };
}

#endif