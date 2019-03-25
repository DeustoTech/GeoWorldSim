#include "GeometryTransformators.h"

#include <QJsonArray>
#include "geos/geom/CoordinateArraySequence.h"
#include "GeometryToGeoJSON.h"

GWSNewGeometry GWSGeometryTransformators::transformMove( const GWSNewGeometry geometry , const GWSCoordinate &apply_movement){
    if( !apply_movement.isValid() ){ return geometry; }

    if( !geometry.inner_geometry ){
        QJsonObject geojson;
        geojson.insert( "type" , "Point" );
        QJsonArray coordinate = { apply_movement.getX() , apply_movement.getY() , apply_movement.getZ() };
        geojson.insert( "coordinates" , coordinate );
        return GWSNewGeometry( geojson );
    }
    // Else
    GWSNewGeometry new_geometry = GWSNewGeometry( geometry );
    GWSGeometryTransformMoveFilter move = GWSGeometryTransformMoveFilter( apply_movement );
    new_geometry.inner_geometry->apply_rw( move );
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( geometry.inner_geometry );
    return new_geometry;
}

GWSNewGeometry GWSGeometryTransformators::transformBuffer( const GWSNewGeometry geometry , double threshold ){
    geos::geom::Geometry* buffered = geometry.inner_geometry->buffer( threshold );
    GWSNewGeometry new_geometry;
    new_geometry.inner_geometry = buffered;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSNewGeometry GWSGeometryTransformators::transformUnion( const GWSNewGeometry geometry , const GWSNewGeometry other){
    geos::geom::Geometry* unioned = geometry.inner_geometry->Union( other.inner_geometry );
    GWSNewGeometry new_geometry;
    new_geometry.inner_geometry = unioned;
    new_geometry.geojson = GWSGeometryToGeoJSON::GeometryToGeoJSON( new_geometry.inner_geometry );
    return new_geometry;
}

GWSNewGeometry GWSGeometryTransformators::transformIntersection( const GWSNewGeometry geometry , const GWSNewGeometry other){
    geos::geom::Geometry* intersected = geometry.inner_geometry->intersection( other.inner_geometry );
    GWSNewGeometry new_geometry;
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
