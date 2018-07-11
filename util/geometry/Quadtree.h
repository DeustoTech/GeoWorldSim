#ifndef GWSQUADTREE_H
#define GWSQUADTREE_H

#include <QObject>
#include <QMutex>

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "geos/index/quadtree/Quadtree.h"

using namespace geos::index;

class GWSQuadtree : public QObject
{
    Q_OBJECT

public:
    explicit GWSQuadtree(QObject *parent = 0);
    ~GWSQuadtree();

    // GETTERS
    QList<void *> getElements( GWSCoordinate coor ) const;
    QList<void *> getElements( GWSEnvelope env ) const;

    // SETTERS
    void insert( GWSCoordinate env , void * elm );
    void insert( GWSEnvelope env , void * elm );
    void remove( GWSCoordinate env , void * elm );
    void remove( GWSEnvelope env , void * elm );

private:
    geos::index::quadtree::Quadtree* inner_index;

    QMutex mutex;
};

#endif // GWSQUADTREE_H
