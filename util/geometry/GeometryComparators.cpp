#include "GeometryComparators.h"

/**********************************************************************
 SPATIAL OPERATORS
**********************************************************************/

bool GWSGeometryComparators::GWSGeometryComparators::isInBounds( const GWSNewGeometry geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geos::geom::Envelope( minX , maxX , minY , maxY ).contains( *geometry.inner_geometry->getEnvelopeInternal() )
              : false;
}

bool GWSGeometryComparators::GWSGeometryComparators::intersects( const GWSNewGeometry geometry , GWSNewGeometry other ){
    if( other.isValid() ){
        return geometry.inner_geometry ? geometry.inner_geometry->intersects( other.inner_geometry ) : false;
    }
    return false;
}

bool GWSGeometryComparators::GWSGeometryComparators::intersects( const GWSNewGeometry geometry , double minX, double maxX, double minY, double maxY){
    return geometry.inner_geometry ?
                geometry.inner_geometry->getEnvelopeInternal()->intersects( geos::geom::Envelope( minX , maxX , minY , maxY ) )
                : false;
}

bool GWSGeometryComparators::GWSGeometryComparators::equals( const GWSNewGeometry geometry , const GWSNewGeometry other ){
    return geometry.inner_geometry ? geometry.inner_geometry->equals( other.inner_geometry ) : false;
}

GWSLengthUnit GWSGeometryComparators::GWSGeometryComparators::getDistance( const GWSNewGeometry geometry , const GWSNewGeometry other ){
    return GWSLengthUnit( geometry.inner_geometry ? geometry.inner_geometry->distance( other.inner_geometry )  * 110574 : -1 );
}
