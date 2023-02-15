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
            using LongLat = boost::geometry::model::d2::point_xy<double, boost::geometry::cs::geographic<boost::geometry::degree>>;

            virtual ~Projection();

            virtual MapPos fromLatLon(const MapPos& pos) const = 0;
            virtual MapPos toLatLon(const MapPos& pos) const = 0;
        /*private:
            template <typename P, typename S>
            MapPos project(S point, P const& proj) {
 
                proj.forward(point, projected_point);
                return projected_point;
            }*/
        protected:
            explicit Projection();
    };

    class EPSG3857 : public Projection {
        public:
            EPSG3857();
            virtual ~EPSG3857();

            virtual MapPos fromLatLon(const MapPos& pos) const;
            virtual MapPos toLatLon(const MapPos& pos) const;
        private:
            const double EPSG3857::EARTH_RADIUS = 6378137.0;
            const double EPSG3857::METERS_TO_INTERNAL_EQUATOR = Const::WORLD_SIZE / (2.0 * Const::PI * EARTH_RADIUS);
    };
}