#include "GeometryTransformators.h"

#include <QJsonArray>
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/simplify/TopologyPreservingSimplifier.h"
#include "GeometryToGeoJSON.h"

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformMove( const geoworldsim::geometry::Geometry &geometry , const geoworldsim::geometry::Coordinate &apply_movement){
    if( !apply_movement.isValid() ){ return geometry; }

    if( !geometry.inner_geometry ){
        QJsonObject geojson;
        geojson.insert( "type" , "Point" );
        QJsonArray coordinate = { apply_movement.getX() , apply_movement.getY() , apply_movement.getZ() };
        geojson.insert( "coordinates" , coordinate );
        return Geometry( geojson );
    }

    // Else
    Geometry new_geometry = Geometry( geometry );
    GeometryTransformMoveFilter move = GeometryTransformMoveFilter( apply_movement );
    new_geometry.inner_geometry->apply_rw( move );
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformBuffer( const geoworldsim::geometry::Geometry &geometry , double threshold ){
    geos::geom::Geometry* buffered = geometry.inner_geometry->buffer( threshold );
    Geometry new_geometry;
    new_geometry.inner_geometry = buffered;
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformUnion( const geoworldsim::geometry::Geometry &geometry , const geoworldsim::geometry::Geometry &other){
    geos::geom::Geometry* unioned = geometry.inner_geometry->Union( other.inner_geometry );
    Geometry new_geometry;
    new_geometry.inner_geometry = unioned;
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformToFit(const geoworldsim::geometry::Geometry &origin, const geoworldsim::geometry::Geometry &extension){
    geos::geom::Geometry* boundary = Q_NULLPTR;
    if( origin.inner_geometry && extension.inner_geometry ) {
        try {
            geos::geom::Geometry* unioned = origin.inner_geometry->Union( extension.inner_geometry );
            boundary = unioned->getEnvelope();
            delete unioned;
        }
        catch(...){ }
    } else if( origin.inner_geometry ) {
        boundary = origin.inner_geometry->clone();
    } else if( extension.inner_geometry ) {
        boundary = extension.inner_geometry->clone();
    }

    if( !boundary ){ return Geometry(); }

    Geometry new_geometry;
    new_geometry.inner_geometry = boundary;
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformIntersection( const geoworldsim::geometry::Geometry &geometry , const geoworldsim::geometry::Geometry &other){
    if( !geometry.inner_geometry ){ return geometry; }
    geos::geom::Geometry* intersected = geometry.inner_geometry->intersection( other.inner_geometry );
    Geometry new_geometry;
    new_geometry.inner_geometry = intersected;
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

geoworldsim::geometry::Geometry geoworldsim::geometry::GeometryTransformators::transformSimplify(const geoworldsim::geometry::Geometry &geometry, double tolerance){
    if( !geometry.inner_geometry ){ return geometry; }
    Geometry new_geometry;
    std::auto_ptr<geos::geom::Geometry> geom = geos::simplify::TopologyPreservingSimplifier::simplify( geometry.inner_geometry , tolerance );
    if( !geom->isValid() ){
        geom.reset();
        return geometry;
    }
    new_geometry.inner_geometry = geom.release();
    new_geometry.geojson = GeometryToGeoJSON::toGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

/**********************************************************************
 SPATIAL TRANSFORMS FILTERS
**********************************************************************/

geoworldsim::geometry::GeometryTransformMoveFilter::GeometryTransformMoveFilter(const geoworldsim::geometry::Coordinate &move ) : geos::geom::CoordinateSequenceFilter() {
    this->apply_movement = move;
}

void geoworldsim::geometry::GeometryTransformMoveFilter::filter_rw( geos::geom::CoordinateSequence& seq , std::size_t i ){
    const geos::geom::Coordinate origin = seq.getAt(i);
    geos::geom::Coordinate moved( origin.x + this->apply_movement.getX() , origin.y + this->apply_movement.getY() , origin.z + this->apply_movement.getZ() );
    seq.setAt(moved, i);
    this->moved_coor_indexes.append( i );
    if( this->moved_coor_indexes.size() == seq.size() ){
        this->finished = true;
    }
}
void geoworldsim::geometry::GeometryTransformMoveFilter::filter_ro( geos::geom::CoordinateSequence &seq, std::size_t i){ Q_UNUSED(seq); Q_UNUSED(i) }
bool geoworldsim::geometry::GeometryTransformMoveFilter::isDone() const { return this->finished; }
bool geoworldsim::geometry::GeometryTransformMoveFilter::isGeometryChanged() const { return true; }
