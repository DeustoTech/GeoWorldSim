#ifndef SNAPTOPOINTFROMEDGE_H
#define SNAPTOPOINTFROMEDGE_H

#include "../../object/Object.h"
#include "../../util/geometry/Coordinate.h"

class SnapToEdgeFromPoint
{

public:
    SnapToEdgeFromPoint( GWSCoordinate coor );

    // METHODS
    GWSCoordinate getCoordinates();

private:

    GWSCoordinate coors_to_snap_to;
};
#endif // SNAPTOPOINTFROMEDGE_H
