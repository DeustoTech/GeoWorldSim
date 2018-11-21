#ifndef MOVETHROUGHROUTESKILL_H
#define MOVETHROUGHROUTESKILL_H

#include <QObject>
#include "../../util/graph/GraphEdge.h"
#include "../../skill/move/MoveSkill.h"

class MoveThroughRouteSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteSkill();
    ~MoveThroughRouteSkill();

    //- PROPERTIES
    static QString SKILL_NETWORK_TYPE_PROP;
    static QString SKILL_ROUTE_DESTINATION_X_PROP;
    static QString SKILL_ROUTE_DESTINATION_Y_PROP;
    static QString AGENT_CURRENT_ROAD_ID_PROP;
    static QString AGENT_CURRENT_ROAD_TYPE_PROP;
    static QString AGENT_CURRENT_ROAD_MAXSPEED_PROP;

    // GETTERS
    GWSCoordinate getRouteDestination();

    // METHODS
    virtual void move(GWSTimeUnit movement_duration);

protected:

    QList< QSharedPointer<GWSGraphEdge> > pending_route;
    QList<GWSCoordinate> pending_edge_coordinates;

    bool route_found_remove = false;

};

#endif // MOVETHROUGHROUTESKILL_H
