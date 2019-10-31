#include "GeometryComparators.h"

/**********************************************************************
 SPATIAL OPERATORS
**********************************************************************/

bool geoworldsim::geometry::GeometryComparators::isInBounds( const Geometry &geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geos::geom::Envelope( minX , maxX , minY , maxY ).contains( *geometry.inner_geometry->getEnvelopeInternal() )
              : false;
}

bool geoworldsim::geometry::GeometryComparators::contains(const Geometry &geometry, const Coordinate &coor){
    return geometry.inner_geometry ?
                geometry.inner_geometry->getEnvelopeInternal()->contains( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) )
              : false;
}

bool geoworldsim::geometry::GeometryComparators::intersects( const Geometry &geometry , const Geometry &other ){
    if( geometry.isValid() && other.isValid() ){
        return geometry.inner_geometry ? geometry.inner_geometry->intersects( other.inner_geometry ) : false;
    }
    return false;
}

bool geoworldsim::geometry::GeometryComparators::intersects( const Geometry &geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geometry.inner_geometry->getEnvelopeInternal()->intersects( geos::geom::Envelope( minX , maxX , minY , maxY ) )
                : false;
}

bool geoworldsim::geometry::GeometryComparators::equals( const Geometry &geometry , const Geometry &other ){
    if( geometry.isValid() && other.isValid() ){
        return geometry.inner_geometry->equals( other.inner_geometry );
    }
    return false;
}

geoworldsim::unit::LengthUnit geoworldsim::geometry::GeometryComparators::getDistance( const Geometry &geometry , const Geometry &other ){
    if( geometry.isValid() && other.isValid() ){
        return unit::LengthUnit( geometry.inner_geometry->distance( other.inner_geometry ) * 110574 );
    }
    return 0;
}

geoworldsim::unit::LengthUnit geoworldsim::geometry::GeometryComparators::getDistance( const Geometry &geometry , const Coordinate &other ){
    if( geometry.isValid() && other.isValid() ){
        Geometry temp = Geometry( other );
        return geoworldsim::geometry::GeometryComparators::getDistance( geometry , temp );
    }
    return 0;
}
