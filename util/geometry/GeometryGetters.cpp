#include "GeometryGetters.h"

#include "geos/geom/CoordinateArraySequence.h"

GWSAreaUnit GWSGeometryGetters::getArea(GWSGeometry geometry){
    return geometry.inner_geometry->getArea();
}

double GWSGeometryGetters::getGeometryMaxX(GWSGeometry geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxX();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMinX(GWSGeometry geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinX();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMaxY(GWSGeometry geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxY();
    }
    return 0;
}

double GWSGeometryGetters::getGeometryMinY(GWSGeometry geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinY();
    }
    return 0;
}

GWSCoordinate GWSGeometryGetters::getCentroid(GWSGeometry geometry){
    if( geometry.isValid() && geometry.inner_geometry ){
        geos::geom::Coordinate centroid;
        geometry.inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    return GWSCoordinate();
}

QList<GWSCoordinate> GWSGeometryGetters::getCoordinates(GWSGeometry geometry){
    QList<GWSCoordinate> coordinates;
    if( geometry.isValid() && geometry.inner_geometry ){
        geos::geom::CoordinateSequence* seq = geometry.inner_geometry->getCoordinates();
        if( seq ){
            for(int i = 0; i < seq->size() ; i++){
                const geos::geom::Coordinate coor = seq->getAt( i );
                coordinates.append( GWSCoordinate( coor.x , coor.y , coor.z ) );
            }
        }
    }
    return coordinates;
}
