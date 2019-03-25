#ifndef GWSGEOMETRYTRANSFORMATORS_H
#define GWSGEOMETRYTRANSFORMATORS_H

#include "NewGeometry.h"

class GWSGeometryTransformators{

    // SPATIAL TRANSFORMATIONS
    static GWSNewGeometry transformMove( const GWSNewGeometry geometry , const GWSCoordinate &apply_movement );
    static GWSNewGeometry transformBuffer( const GWSNewGeometry geometry , double threshold );
    static GWSNewGeometry transformUnion( const GWSNewGeometry geometry , const GWSNewGeometry other );
    static GWSNewGeometry transformIntersection( const GWSNewGeometry geometry , const GWSNewGeometry other );

};

#endif // GWSGEOMETRYTRANSFORMATORS_H
