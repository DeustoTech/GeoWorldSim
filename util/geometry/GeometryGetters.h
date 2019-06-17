#ifndef GWSGEOMETRYGETTERS_H
#define GWSGEOMETRYGETTERS_H

#include "Geometry.h"
#include "Coordinate.h"
#include "../../util/units/Units.h"

class GWSGeometryGetters {

public:
    static GWSAreaUnit getArea( const GWSGeometry &geometry );
    static double getGeometryMaxX( const GWSGeometry &geometry );
    static double getGeometryMinX( const GWSGeometry &geometry );
    static double getGeometryMaxY( const GWSGeometry &geometry );
    static double getGeometryMinY( const GWSGeometry &geometry );
    static GWSCoordinate getCentroid( const GWSGeometry &geometry );
    static QList<GWSCoordinate> getCoordinates( const GWSGeometry &geometry );

};

#endif // GWSGEOMETRYGETTERS_H
