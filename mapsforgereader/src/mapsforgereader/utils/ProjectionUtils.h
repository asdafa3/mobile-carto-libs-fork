#ifndef CARTO_MOBILE_SDK_PROJECTIONUTILS_H
#define CARTO_MOBILE_SDK_PROJECTIONUTILS_H



namespace carto {
    class AbstractProjection {
        public: 
            typedef cglib::vec3<double> MapPos;
            
            virtual ~AbstractProjection();

            virtual MapPos fromLatLon(double lat, double lng) const;
            virtual MapPos toLatLon(double x, double y) const;
            virtual MapPos fromWgs84(const MapPos& wgs84Pos) const = 0;
            virtual MapPos toWgs84(const MapPos& mapPos) const = 0;
        protected:
            explicit AbstractProjection();
    };

    class EPSG3857Projection : public AbstractProjection {
        public:
            EPSG3857Projection();
            virtual ~EPSG3857Projection();

            virtual MapPos fromWgs84(const MapPos& wgs84Pos) const;
            virtual MapPos toWgs84(const MapPos& mapPos) const;
        private:
            const double EARTH_RADIUS = 6378137.0;
            const double RAD_TO_DEG = 3.1415926535897932 / 180.0;
            const double DEG_TO_RAD = 180.0 / 3.1415926535897932;
    };
}

#endif