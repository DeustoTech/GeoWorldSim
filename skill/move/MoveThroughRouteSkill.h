#ifndef MOVETHROUGHROUTESKILL_H
#define MOVETHROUGHROUTESKILL_H

#include <QObject>
#include "../../skill/move/MoveSkill.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../util/graph/Graph.h"


class MoveThroughRouteSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteSkill();
    ~MoveThroughRouteSkill();

    //- PROPERTIES
    static QString EDGES_CLASS_PROP;
    static QString ROUTE_DESTINATION_X_PROP;
    static QString ROUTE_DESTINATION_Y_PROP;

    // GETTERS
    GWSCoordinate getRouteDestination() const;

    // METHODS
    virtual void generateGraph();
    virtual void move(GWSTimeUnit movement_duration);

protected:

    GWSDijkstraRouting* routing_graph = Q_NULLPTR;
    QList< QSharedPointer<GWSGraphEdge> > pending_route;

};

#endif // MOVETHROUGHROUTESKILL_H
