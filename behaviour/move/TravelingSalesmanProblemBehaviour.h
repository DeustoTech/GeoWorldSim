#ifndef TRAVELINGSALESMANPROBLEMBEHAVIOUR_H
#define TRAVELINGSALESMANPROBLEMBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../util/graph/Graph.h"

class TravelingSalesmanProblemBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    TravelingSalesmanProblemBehaviour();

    //- PROPERTIES
    static QString EDGES_CLASS_PROP;

private slots:
    virtual bool behave();
    // GETTERS
    virtual bool continueToNext();

protected:
    GWSDijkstraRouting* routing_graph = Q_NULLPTR;
    QList< QSharedPointer<GWSGraphEdge> > route;
};

#endif // TRAVELINGSALESMANPROBLEMBEHAVIOUR_H
