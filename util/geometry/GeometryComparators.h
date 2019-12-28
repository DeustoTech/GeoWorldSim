#ifndef GEOMETRYCOMPARATORS_H
#define GEOMETRYCOMPARATORS_H

#include "Geometry.h"
#include "../../util/units/Units.h"


namespace geoworldsim {
namespace geometry {


class GeometryComparators {

public:
    static bool isInBounds( const Geometry &geometry , double minX , double maxX , double minY , double maxY );
    static bool contains( const Geometry &geometry , const Coordinate &coor );
    static bool intersects( const Geometry &geometry , const Geometry &other);
    static bool intersects( const Geometry &geometry , double minX , double maxX , double minY , double maxY );
    static bool equals( const Geometry &geometry , const Geometry &other );
    static unit::LengthUnit getDistance( const Geometry &geometry , const Geometry& other );
    static unit::LengthUnit getDistance( const Geometry &geometry , const Coordinate& other );

};

}
}

#endif // GEOMETRYCOMPARATORS_H
