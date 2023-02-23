#include <mapnikvt/Types.h>
#include <boost/geometry.hpp>
#include <boost/geometry/srs/epsg.hpp>
#include <boost/geometry/srs/projection.hpp>
#include <boost/geometry/core/coordinate_system.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

namespace carto {
    class Projection {
        public: 
            using MapPos = carto::mvt::MapPos;

            virtual ~Projection();

            virtual MapPos fromLatLon(double lat, double lng) const;
            virtual MapPos toLatLon(double x, double y) const;
            virtual MapPos fromWgs84(const MapPos& pos) const = 0;
            virtual MapPos toWgs84(const MapPos& pos) const = 0;
        protected:
            explicit Projection();
    };

    class EPSG3857 : public Projection {
        public:
            EPSG3857();
            virtual ~EPSG3857();

            virtual MapPos fromWgs84(const MapPos& pos) const;
            virtual MapPos toWgs84(const MapPos& pos) const;
        private:
            const double EPSG3857::EARTH_RADIUS = 6378137.0;
    };
}