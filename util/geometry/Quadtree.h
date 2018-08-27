#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QMutex>
#include <QMap>

#include "../../object/Object.h"
#include "../../agent/Agent.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"
#include "geos/index/quadtree/Quadtree.h"

using namespace geos::index;

class GWSQuadtree : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    QList<GWSAgent*> getElements( GWSCoordinate coor ) const;
    QList<GWSAgent*> getElements( const GWSGeometry* geometry ) const;
    QList<GWSAgent*> getElements( double minX, double maxX, double minY, double maxY ) const;
    GWSAgent* getNearestElement( GWSCoordinate coor ) const;
    GWSAgent* getNearestElement( GWSGeometry* geometry ) const;

    // SETTERS
    void upsert( GWSAgent* agent );
    void remove( GWSAgent* agent );

private:

    geos::index::quadtree::Quadtree* inner_index;
    QMap<GWSAgent* , geos::geom::Envelope> registered_envelopes;

    QMutex mutex;
};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
