#ifndef GWSIMAGECOORDINATESCONVERSOR_H
#define GWSIMAGECOORDINATESCONVERSOR_H

#include <QPoint>

class ImageCoordinatesConversor
{

public:

    // METHODS
    static QPoint reprojectPoint(double lat, double lon, double max_lat, double min_lat, double max_lon, double min_lon , int image_x_size, int image_y_size);

    static unsigned int lon2x( double lon , double min_lon, double max_lon , unsigned int image_x_size); // Given the longitude, left and right image boundaries, get the X pixel
    static unsigned int lat2y( double lat , double min_lat , double max_lat , unsigned int image_y_size ); // Given the latitude, top and bottom image boundaries, get the Y pixel

};


#endif // GWSIMAGECOORDINATESCONVERSOR_H
