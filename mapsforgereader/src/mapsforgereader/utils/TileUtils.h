#ifndef CARTO_MOBILE_SDK_PROJECTIONUTILS_H
#define CARTO_MOBILE_SDK_PROJECTIONUTILS_H



namespace carto {
    class TileUtils {
        public: 
            
            

            virtual ~TileUtils();

            virtual MapPos CalculateMapTile(double lat, double lng) const;
            virtual MapPos CalculateMapTile(double x, double y) const;
        protected:
            explicit TileUtils();
    };
}

#endif