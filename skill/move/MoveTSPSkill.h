#ifndef TSPSKILL_H
#define TSPSKILL_H


#include <QObject>
#include "../../skill/move/MoveSkill.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../util/graph/Graph.h"

class MoveTSPSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveTSPSkill();
    ~MoveTSPSkill();

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
    QList<QList< QSharedPointer<GWSGraphEdge> > > route;

};

#endif // TSPSKILL_H
