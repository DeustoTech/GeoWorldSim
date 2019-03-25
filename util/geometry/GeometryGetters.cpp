#include "GeometryGetters.h"

#include "geos/geom/CoordinateArraySequence.h"

GWSAreaUnit GWSGeometryGetters::getArea(GWSNewGeometry geometry){
    return geometry.inner_geometry->getArea();
}

double GWSGeometryGetters::getGeometryMaxX(GWSNewGeometry geometry){
    return geometry.inner_geometry->getEnvelopeInternal()->getMaxX();
}

double GWSGeometryGetters::getGeometryMinX(GWSNewGeometry geometry){
    return geometry.inner_geometry->getEnvelopeInternal()->getMinX();
}

double GWSGeometryGetters::getGeometryMaxY(GWSNewGeometry geometry){
    return geometry.inner_geometry->getEnvelopeInternal()->getMaxY();
}

double GWSGeometryGetters::getGeometryMinY(GWSNewGeometry geometry){
    return geometry.inner_geometry->getEnvelopeInternal()->getMinY();
}

GWSCoordinate GWSGeometryGetters::getCentroid(GWSNewGeometry geometry){
    if( geometry.isValid() && geometry.inner_geometry ){
        geos::geom::Coordinate centroid;
        geometry.inner_geometry->getCentroid( centroid );
        return GWSCoordinate( centroid.x , centroid.y , centroid.z );
    }
    return GWSCoordinate();
}

QList<GWSCoordinate> GWSGeometryGetters::getCoordinates(GWSNewGeometry geometry){
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
