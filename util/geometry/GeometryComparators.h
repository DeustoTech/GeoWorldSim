#ifndef GWSGEOMETRYCOMPARATORS_H
#define GWSGEOMETRYCOMPARATORS_H

#include "NewGeometry.h"
#include "../../util/units/Units.h"

class GWSGeometryComparators {

    static bool isInBounds( const GWSNewGeometry geometry , double minX , double maxX , double minY , double maxY );
    static bool intersects( const GWSNewGeometry geometry , const GWSNewGeometry other);
    static bool intersects( const GWSNewGeometry geometry , double minX , double maxX , double minY , double maxY );
    static bool equals( const GWSNewGeometry geometry , GWSNewGeometry other );
    static GWSLengthUnit getDistance( const GWSNewGeometry geometry , GWSNewGeometry other );

};

#endif // GWSGEOMETRYCOMPARATORS_H
