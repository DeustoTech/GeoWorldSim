#ifndef GWSGEOMETRYGETTERS_H
#define GWSGEOMETRYGETTERS_H

#include "Geometry.h"
#include "Coordinate.h"
#include "../../util/units/Units.h"

class GWSGeometryGetters {

public:
    static GWSAreaUnit getArea( GWSGeometry geometry );
    static double getGeometryMaxX( GWSGeometry geometry );
    static double getGeometryMinX( GWSGeometry geometry );
    static double getGeometryMaxY( GWSGeometry geometry );
    static double getGeometryMinY( GWSGeometry geometry );
    static GWSCoordinate getCentroid( GWSGeometry geometry );
    static QList<GWSCoordinate> getCoordinates( GWSGeometry geometry );

};

#endif // GWSGEOMETRYGETTERS_H
