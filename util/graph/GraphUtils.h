#ifndef GSSGRAPHUTILS_H
#define GSSGRAPHUTILS_H

#include <QObject>

#include "../../util/graph/Graph.h"
#include "../../util/geometry/Quadtree.h"

class GWSGraphUtils : public QObject
{
    Q_OBJECT

public:

    static GWSGraphUtils* globalInstance();

    // METHODS
    QMap<GWSGraphEdge* , QList<GWSGraphEdge*> >  createSimplification( const GWSGraph* graph );
    GWSGraphNode* findNearestFromQuadtree( GWSCoordinate coor , const GWSQuadtree* nodes_index );

private:
    explicit GWSGraphUtils(QObject *parent = 0);
    GWSGraphUtils(GWSGraphUtils const&);
    ~GWSGraphUtils();
};

#endif // GSSGRAPHUTILS_H
