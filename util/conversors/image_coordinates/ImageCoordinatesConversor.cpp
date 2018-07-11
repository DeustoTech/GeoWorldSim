#include "ImageCoordinatesConversor.h"
#include <QtMath>

ImageCoordinatesConversor::ImageCoordinatesConversor(QObject *parent) : QObject(parent){

}

/**********************************************************************
 GETTERS
**********************************************************************/

QPoint ImageCoordinatesConversor::reprojectPoint(double lat, double lon, double max_lat, double min_lat, double max_lon, double min_lon, int image_x_size, int image_y_size){
    double x = ImageCoordinatesConversor::lon2x( lon , min_lon , max_lon , image_x_size );
    double y = ImageCoordinatesConversor::lat2y( lat , min_lat , max_lat , image_y_size );
    return QPoint( x , y );
}

unsigned int ImageCoordinatesConversor::lon2x(double lon, double min_lon, double max_lon, unsigned int image_x_size){
    double difference = qAbs( min_lon - max_lon );
    return ((lon - qMin( min_lon , max_lon ) ) / difference) * image_x_size;
}

unsigned int ImageCoordinatesConversor::lat2y(double lat, double min_lat, double max_lat, unsigned int image_y_size){
    double difference = qAbs( max_lat - min_lat );
    // Rendering images pixel 0 is in top, so 1-value
    return ( 1 - ((lat - qMin( min_lat , max_lat ) ) / difference) ) * image_y_size;
}
