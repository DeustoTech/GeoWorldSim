#ifndef GWSGRIDCOORDINATESCONVERSOR_H
#define GWSGRIDCOORDINATESCONVERSOR_H

class GWSGridCoordinatesConversor
{

public:

    // GETTERS
    static unsigned int lon2x( double lon , double min_lon, double max_lon , unsigned int x_size); // Given the longitude, left and right boundaries, get the X  of grid
    static unsigned int lat2y( double lat , double min_lat , double max_lat , unsigned int y_size ); // Given the latitude, top and bottom boundaries, get the Y of grid

    static double x2lon(unsigned int grid_x, double min_lon, double max_lon , unsigned int x_size);
    static double y2lat(unsigned int grid_y, double min_lat , double max_lat , unsigned int y_size );
};

#endif // GWSGRIDCOORDINATESCONVERSOR_H
