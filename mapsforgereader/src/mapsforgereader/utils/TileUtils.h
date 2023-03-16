#ifndef CARTO_MOBILE_SDK_PROJECTIONUTILS_H
#define CARTO_MOBILE_SDK_PROJECTIONUTILS_H

#include <mapnikvt/Types.h>

namespace carto {
    class TileUtils {
        public: 
            
            

            virtual ~TileUtils();

            virtual MapPos Calculatemvt::MapTile(double lat, double lng) const;
            virtual MapPos Calculatemvt::MapTile(double x, double y) const;
        protected:
            explicit TileUtils();
    };
}

#endif