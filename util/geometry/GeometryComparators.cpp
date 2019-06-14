#include "GeometryComparators.h"

/**********************************************************************
 SPATIAL OPERATORS
**********************************************************************/

bool GWSGeometryComparators::GWSGeometryComparators::isInBounds( const GWSGeometry &geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geos::geom::Envelope( minX , maxX , minY , maxY ).contains( *geometry.inner_geometry->getEnvelopeInternal() )
              : false;
}

bool GWSGeometryComparators::GWSGeometryComparators::contains(const GWSGeometry &geometry, const GWSCoordinate &coor){
    return geometry.inner_geometry ?
                geometry.inner_geometry->getEnvelopeInternal()->contains( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) )
              : false;
}

bool GWSGeometryComparators::GWSGeometryComparators::intersects( const GWSGeometry &geometry , const GWSGeometry &other ){
    if( geometry.isValid() && other.isValid() ){
        return geometry.inner_geometry ? geometry.inner_geometry->intersects( other.inner_geometry ) : false;
    }
    return false;
}

bool GWSGeometryComparators::GWSGeometryComparators::intersects( const GWSGeometry &geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geometry.inner_geometry->getEnvelopeInternal()->intersects( geos::geom::Envelope( minX , maxX , minY , maxY ) )
                : false;
}

bool GWSGeometryComparators::GWSGeometryComparators::equals( const GWSGeometry &geometry , const GWSGeometry &other ){
    return geometry.inner_geometry ? geometry.inner_geometry->equals( other.inner_geometry ) : false;
}

GWSLengthUnit GWSGeometryComparators::GWSGeometryComparators::getDistance( const GWSGeometry &geometry , const GWSGeometry &other ){
    if( geometry.isValid() && other.isValid() ){
        return GWSLengthUnit( geometry.inner_geometry->distance( other.inner_geometry ) * 110574 );
    }
    return -1;
}
