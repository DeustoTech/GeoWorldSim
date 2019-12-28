#ifndef GEOMETRYGETTERS_H
#define GEOMETRYGETTERS_H

#include "Geometry.h"
#include "Coordinate.h"
#include "../../util/units/Units.h"

namespace geoworldsim {
namespace geometry {


class GeometryGetters {

public:
    static unit::AreaUnit getArea( const Geometry &geometry );
    static double getGeometryMaxX( const Geometry &geometry );
    static double getGeometryMinX( const Geometry &geometry );
    static double getGeometryMaxY( const Geometry &geometry );
    static double getGeometryMinY( const Geometry &geometry );
    static Coordinate getCentroid( const Geometry &geometry );
    static QList<Coordinate> getCoordinates( const Geometry &geometry );

};

}
}

#endif // GEOMETRYGETTERS_H
