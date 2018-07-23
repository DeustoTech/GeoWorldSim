#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QMutex>

#include "../../object/Object.h"
#include "../../agent/Agent.h"
#include "../../util/geometry/Point.h"
#include "../../util/geometry/Polygon.h"
#include "geos/index/quadtree/Quadtree.h"

using namespace geos::index;

class GWSQuadtree : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSQuadtree();
    ~GWSQuadtree();

    // GETTERS
    QList<GWSAgent*> getElements( GWSPoint point ) const;
    QList<GWSAgent*> getElements( GWSPolygon env ) const;

    // SETTERS
    void insert( GWSAgent* agent );
    void remove( GWSAgent* agent );

private:
    geos::index::quadtree::Quadtree* inner_index;

    //QMutex mutex;
};

Q_DECLARE_METATYPE(GWSQuadtree)

#endif // GWSQUADTREE_H
