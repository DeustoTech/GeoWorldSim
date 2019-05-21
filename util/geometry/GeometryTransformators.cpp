#include "GeometryTransformators.h"

#include <QJsonArray>
#include "geos/geom/CoordinateArraySequence.h"
#include "GeometryToGeoJSON.h"

GWSGeometry GWSGeometryTransformators::transformMove( const GWSGeometry geometry , const GWSCoordinate &apply_movement){
    if( !apply_movement.isValid() ){ return geometry; }

    if( !geometry.inner_geometry ){
        QJsonObject geojson;
        geojson.insert( "type" , "Point" );
        QJsonArray coordinate = { apply_movement.getX() , apply_movement.getY() , apply_movement.getZ() };
        geojson.insert( "coordinates" , coordinate );
        return GWSGeometry( geojson );
    }

    // Else
    GWSGeometry new_geometry = GWSGeometry( geometry );
    GWSGeometryTransformMoveFilter move = GWSGeometryTransformMoveFilter( apply_movement );
    new_geometry.inner_geometry->apply_rw( move );
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSGeometry GWSGeometryTransformators::transformBuffer( const GWSGeometry geometry , double threshold ){
    geos::geom::Geometry* buffered = geometry.inner_geometry->buffer( threshold );
    GWSGeometry new_geometry;
    new_geometry.inner_geometry = buffered;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSGeometry GWSGeometryTransformators::transformUnion( const GWSGeometry geometry , const GWSGeometry other){
    geos::geom::Geometry* unioned = geometry.inner_geometry->Union( other.inner_geometry );
    GWSGeometry new_geometry;
    new_geometry.inner_geometry = unioned;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSGeometry GWSGeometryTransformators::transformToFit(const GWSGeometry origin, const GWSGeometry extension){
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

    if( !boundary ){ return GWSGeometry(); }

    GWSGeometry new_geometry;
    new_geometry.inner_geometry = boundary;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSGeometry GWSGeometryTransformators::transformIntersection( const GWSGeometry geometry , const GWSGeometry other){
    geos::geom::Geometry* intersected = geometry.inner_geometry->intersection( other.inner_geometry );
    GWSGeometry new_geometry;
    new_geometry.inner_geometry = intersected;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

/**********************************************************************
 SPATIAL TRANSFORMS FILTERS
**********************************************************************/

GWSGeometryTransformMoveFilter::GWSGeometryTransformMoveFilter(const GWSCoordinate &move ) : geos::geom::CoordinateSequenceFilter() {
    this->apply_movement = move;
}

void GWSGeometryTransformMoveFilter::filter_rw(CoordinateSequence& seq , std::size_t i ){
    const geos::geom::Coordinate origin = seq.getAt(i);
    geos::geom::Coordinate moved( origin.x + this->apply_movement.getX() , origin.y + this->apply_movement.getY() , origin.z + this->apply_movement.getZ() );
    seq.setAt(moved, i);
    this->moved_coor_indexes.append( i );
    if( this->moved_coor_indexes.size() == seq.size() ){
        this->finished = true;
    }
}
void GWSGeometryTransformMoveFilter::filter_ro(CoordinateSequence &seq, std::size_t i){}
bool GWSGeometryTransformMoveFilter::isDone() const { return this->finished; }
bool GWSGeometryTransformMoveFilter::isGeometryChanged() const { return true; }
