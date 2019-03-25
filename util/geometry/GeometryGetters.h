#ifndef GWSGEOMETRYGETTERS_H
#define GWSGEOMETRYGETTERS_H

#include "NewGeometry.h"
#include "Coordinate.h"
#include "../../util/units/Units.h"

class GWSGeometryGetters {

public:
    static GWSAreaUnit getArea( GWSNewGeometry geometry );
    static double getGeometryMaxX( GWSNewGeometry geometry );
    static double getGeometryMinX( GWSNewGeometry geometry );
    static double getGeometryMaxY( GWSNewGeometry geometry );
    static double getGeometryMinY( GWSNewGeometry geometry );
    static GWSCoordinate getCentroid( GWSNewGeometry geometry );
    static QList<GWSCoordinate> getCoordinates( GWSNewGeometry geometry );

};

#endif // GWSGEOMETRYGETTERS_H
