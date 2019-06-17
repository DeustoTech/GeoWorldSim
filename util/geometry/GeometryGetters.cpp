#include "GeometryGetters.h"

#include "geos/geom/CoordinateArraySequence.h"

GWSAreaUnit GWSGeometryGetters::getArea(const GWSGeometry &geometry){
    return geometry.inner_geometry->getArea();
}

double GWSGeometryGetters::getGeometryMaxX(const GWSGeometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxX();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMinX(const GWSGeometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinX();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMaxY(const GWSGeometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxY();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMinY(const GWSGeometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinY();
    }
    return 0;
}

GWSCoordinate GWSGeometryGetters::getCentroid(const GWSGeometry &geometry){
    if( geometry.isValid() ){
        geos::geom::Coordinate centroid;
        geometry.inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    return GWSCoordinate();
}

QList<GWSCoordinate> GWSGeometryGetters::getCoordinates(const GWSGeometry &geometry){
    QList<GWSCoordinate> coordinates;
    if( geometry.isValid() && geometry.inner_geometry ){
        geos::geom::CoordinateSequence* seq = geometry.inner_geometry->getCoordinates();
        if( seq ){
            for(unsigned int i = 0; i < seq->size() ; i++){
                const geos::geom::Coordinate coor = seq->getAt( i );
                coordinates.append( GWSCoordinate( coor.x , coor.y , coor.z ) );
            }
        }
    }
    return coordinates;
}
