#include "GeometryTransformators.h"
#include "GeometryToGeoJSON.h"

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
