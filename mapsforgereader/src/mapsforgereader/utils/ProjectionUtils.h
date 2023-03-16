#ifndef CARTO_MOBILE_SDK_PROJECTIONUTILS_H
#define CARTO_MOBILE_SDK_PROJECTIONUTILS_H

#include <mapnikvt/Types.h>

namespace carto {
    class Projection {
        public: 
            

            virtual ~Projection();

            virtual MapPos fromLatLon(double lat, double lng) const;
            virtual MapPos toLatLon(double x, double y) const;
            virtual MapPos fromWgs84(const MapPos& wgs84Pos) const = 0;
            virtual MapPos toWgs84(const MapPos& mapPos) const = 0;
        protected:
            explicit Projection();
    };

    class EPSG3857 : public Projection {
        public:
            EPSG3857();
            virtual ~EPSG3857();

            virtual MapPos fromWgs84(const MapPos& wgs84Pos) const;
            virtual MapPos toWgs84(const MapPos& mapPos) const;
        private:
            const double EPSG3857::EARTH_RADIUS = 6378137.0;
            const double EPSG3857::RAD_TO_DEG = 3.1415926535897932 / 180.0;
            const double EPSG3857::DEG_TO_RAD = 180.0 / 3.1415926535897932;
    };
}

#endif