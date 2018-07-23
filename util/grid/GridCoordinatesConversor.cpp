#include "GridCoordinatesConversor.h"

#include <QDebug>
#include <QtMath>


/**********************************************************************
 GETTERS
**********************************************************************/

unsigned int GWSGridCoordinatesConversor::lon2x(double lon, double min_lon, double max_lon, unsigned int x_size){
    if( lon < min_lon || lon > max_lon ){
        qWarning() << "Longitude outside left-right bounds";
        return 0;
    }
    double lon_diff = qAbs(max_lon - min_lon);
    unsigned int x = qFloor( (lon - min_lon) * x_size / lon_diff);
    return x;
}

unsigned int GWSGridCoordinatesConversor::lat2y(double lat, double min_lat, double max_lat, unsigned int y_size){
    if( lat < min_lat || lat > max_lat ){
        qWarning() << "Latitude outside top-bottom bounds";
        return 0;
    }
    double lat_diff = qAbs(max_lat - min_lat);
    unsigned int y = qFloor( (lat - min_lat) * y_size / lat_diff);
    return y;
}

/** Returns longitude of the left side
 * @brief GridCoordinatesConversor::x2lon
 * @param grid_x
 * @param min_lon
 * @param max_lon
 * @param x_size
 * @return
 */
double GWSGridCoordinatesConversor::x2lon(unsigned int grid_x, double min_lon, double max_lon, unsigned int x_size){
    if( grid_x < 0 || grid_x > x_size ){
        qWarning() << "Longitude outside left-right bounds";
        return 0;
    }

    double lon_diff = qAbs(max_lon - min_lon);
    double lon = min_lon + lon_diff / x_size * grid_x;
    return lon;
}

/** Returns latitude of the bottom side
 * @brief GridCoordinatesConversor::y2lat
 * @param grid_y
 * @param min_lat
 * @param max_lat
 * @param y_size
 * @return
 */
double GWSGridCoordinatesConversor::y2lat(unsigned int grid_y, double min_lat, double max_lat, unsigned int y_size){
    if( grid_y < 0 || grid_y > y_size ){
        qWarning() << "Latitude outside top-bottom bounds";
        return 0;
    }

    double lat_diff = qAbs(max_lat - min_lat);
    double lat = min_lat + lat_diff / y_size * grid_y;
    return lat;
}
