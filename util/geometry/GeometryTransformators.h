#ifndef GEOMETRYTRANSFORMATORS_H
#define GEOMETRYTRANSFORMATORS_H

#include "geos/geom/CoordinateSequenceFilter.h"
#include "Geometry.h"

namespace geoworldsim {
namespace geometry {


class GeometryTransformators{

public:

    static Geometry transformMove( const Geometry &geometry , const Coordinate &apply_movement );
    static Geometry transformBuffer( const Geometry &geometry , double threshold );
    static Geometry transformUnion( const Geometry &geometry , const Geometry &other );
    static Geometry transformToFit( const Geometry &origin , const Geometry &extension );
    static Geometry transformIntersection( const Geometry &geometry , const Geometry &other );
    static Geometry transformSimplify( const Geometry &geometry , double tolerance );

};

// SPATIAL TRANSFORM FILTERS
class GeometryTransformMoveFilter : public geos::geom::CoordinateSequenceFilter{
public:
    GeometryTransformMoveFilter(const Coordinate &apply_movement );
    virtual void filter_rw(geos::geom::CoordinateSequence& seq , std::size_t i );
    virtual void filter_ro(geos::geom::CoordinateSequence&  seq , std::size_t i );
    virtual bool isDone() const;
    virtual bool isGeometryChanged() const;
    Coordinate apply_movement;
    QVector<int> moved_coor_indexes;
    bool finished = false;
};

}
}

#endif // GEOMETRYTRANSFORMATORS_H
