#ifndef GWSGEOMETRYCOMPARATORS_H
#define GWSGEOMETRYCOMPARATORS_H

#include "Geometry.h"
#include "../../util/units/Units.h"

class GWSGeometryComparators {

public:
    static bool isInBounds( const GWSGeometry &geometry , double minX , double maxX , double minY , double maxY );
    static bool contains( const GWSGeometry &geometry , const GWSCoordinate &coor );
    static bool intersects( const GWSGeometry &geometry , const GWSGeometry &other);
    static bool intersects( const GWSGeometry &geometry , double minX , double maxX , double minY , double maxY );
    static bool equals( const GWSGeometry &geometry , const GWSGeometry &other );
    static GWSLengthUnit getDistance( const GWSGeometry &geometry , const GWSGeometry &other );

};

#endif // GWSGEOMETRYCOMPARATORS_H
