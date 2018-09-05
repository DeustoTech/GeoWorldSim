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
    QList< QSharedPointer<GWSAgent> > getElements( GWSCoordinate coor ) const;
    QList< QSharedPointer<GWSAgent> > getElements( QSharedPointer<GWSGeometry> geometry ) const;
    QList< QSharedPointer<GWSAgent> > getElements( double minX, double maxX, double minY, double maxY ) const;
    QSharedPointer<GWSAgent> getNearestElement( GWSCoordinate coor ) const;
    QSharedPointer<GWSAgent> getNearestElement( QSharedPointer<GWSGeometry> geometry ) const;

    // SETTERS
    void upsert( QSharedPointer<GWSAgent> agent , GWSCoordinate coor );
    void upsert( QSharedPointer<GWSAgent> agent , QSharedPointer<GWSGeometry> geom );
    void remove( QSharedPointer<GWSAgent> agent );

private:

    geos::index::quadtree::Quadtree* inner_index;
    QMap< QSharedPointer<GWSAgent> , geos::geom::Envelope> registered_envelopes;

    QMutex mutex;
};

Q_DECLARE_METATYPE(GWSQuadtree*)

#endif // GWSQUADTREE_H
