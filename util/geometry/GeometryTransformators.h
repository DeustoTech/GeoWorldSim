#ifndef GWSGEOMETRYTRANSFORMATORS_H
#define GWSGEOMETRYTRANSFORMATORS_H

#include "geos/geom/CoordinateSequenceFilter.h"

#include "Geometry.h"

class GWSGeometryTransformators{

public:
    static GWSGeometry transformMove( const GWSGeometry geometry , const GWSCoordinate &apply_movement );
    static GWSGeometry transformBuffer( const GWSGeometry geometry , double threshold );
    static GWSGeometry transformUnion( const GWSGeometry geometry , const GWSGeometry other );
    static GWSGeometry transformToFit( const GWSGeometry origin , const GWSGeometry extension );
    static GWSGeometry transformIntersection( const GWSGeometry geometry , const GWSGeometry other );

};

// SPATIAL TRANSFORM FILTERS
class GWSGeometryTransformMoveFilter : public geos::geom::CoordinateSequenceFilter{
public:
    GWSGeometryTransformMoveFilter(const GWSCoordinate &apply_movement );
    virtual void filter_rw(CoordinateSequence&  seq , std::size_t i );
    virtual void filter_ro(CoordinateSequence&  seq , std::size_t i );
    virtual bool isDone() const;
    virtual bool isGeometryChanged() const;
    GWSCoordinate apply_movement;
    QVector<int> moved_coor_indexes;
    bool finished = false;
};

#endif // GWSGEOMETRYTRANSFORMATORS_H
