#ifndef GWSGEOMETRYTRANSFORMATORS_H
#define GWSGEOMETRYTRANSFORMATORS_H

#include "geos/geom/CoordinateSequenceFilter.h"

#include "NewGeometry.h"

class GWSGeometryTransformators{

public:
    static GWSNewGeometry transformMove( const GWSNewGeometry geometry , const GWSCoordinate &apply_movement );
    static GWSNewGeometry transformBuffer( const GWSNewGeometry geometry , double threshold );
    static GWSNewGeometry transformUnion( const GWSNewGeometry geometry , const GWSNewGeometry other );
    static GWSNewGeometry transformIntersection( const GWSNewGeometry geometry , const GWSNewGeometry other );

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
