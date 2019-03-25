#ifndef MOVETHROUGHROUTESKILL_H
#define MOVETHROUGHROUTESKILL_H

#include <QObject>
#include "../../environment/network_environment/NetworkEdge.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/pollute/VehiclePolluteSkill.h"

class MoveThroughRouteSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteSkill();
    ~MoveThroughRouteSkill();

    //- PROPERTIES
    static QString EDGE_CAPACITY_PROP;
    static QString EDGE_INSIDE_AGENT_IDS_PROP;
    static QString STORE_CURRENT_ROAD_ID;
    static QString STORE_CURRENT_ROAD_TYPE;
    static QString STORE_CURRENT_ROAD_MAXSPEED;

    // GETTERS
    //GWSCoordinate getRouteDestination();

    // METHODS
    virtual void move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate route_destination , QString graph_type );

protected:

    QList<GWSNetworkEdge> pending_route;
    QList<GWSCoordinate> pending_edge_coordinates;
    GWSCoordinate last_route_started_from;

};

#endif // MOVETHROUGHROUTESKILL_H
