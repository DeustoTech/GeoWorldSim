#include "GeometryGetters.h"

#include "geos/geom/CoordinateArraySequence.h"

geoworldsim::unit::AreaUnit geoworldsim::geometry::GeometryGetters::getArea(const geoworldsim::geometry::Geometry &geometry){
    return geometry.inner_geometry->getArea();
}

double geoworldsim::geometry::GeometryGetters::getGeometryMaxX(const geoworldsim::geometry::Geometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxX();
    }
    return 0;
}

double geoworldsim::geometry::GeometryGetters::getGeometryMinX(const geoworldsim::geometry::Geometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinX();
    }
    return 0;
}

double geoworldsim::geometry::GeometryGetters::getGeometryMaxY(const geoworldsim::geometry::Geometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMaxY();
    }
    return 0;
}

double geoworldsim::geometry::GeometryGetters::getGeometryMinY(const geoworldsim::geometry::Geometry &geometry){
    if( geometry.inner_geometry && geometry.inner_geometry->getEnvelopeInternal() ){
        return geometry.inner_geometry->getEnvelopeInternal()->getMinY();
    }
    return 0;
}

geoworldsim::geometry::Coordinate geoworldsim::geometry::GeometryGetters::getCentroid(const geoworldsim::geometry::Geometry &geometry){
    if( geometry.isValid() ){
        geos::geom::Coordinate centroid;
        geometry.inner_geometry->getCentroid( centroid );
        return Coordinate( centroid.x , centroid.y , centroid.z );
    }
    return Coordinate();
}

QList<geoworldsim::geometry::Coordinate> geoworldsim::geometry::GeometryGetters::getCoordinates(const geoworldsim::geometry::Geometry &geometry){
    QList<Coordinate> coordinates;
    if( geometry.isValid() && geometry.inner_geometry ){
        geos::geom::CoordinateSequence* seq = geometry.inner_geometry->getCoordinates();
        if( seq ){
            for(unsigned int i = 0; i < seq->size() ; i++){
                const geos::geom::Coordinate coor = seq->getAt( i );
                coordinates.append( Coordinate( coor.x , coor.y , coor.z ) );
            }
        }
    }
    return coordinates;
}
