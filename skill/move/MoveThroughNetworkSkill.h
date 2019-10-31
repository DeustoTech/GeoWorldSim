#ifndef MOVETHROUGHNETWORKSKILL_H
#define MOVETHROUGHNETWORKSKILL_H

#include <QObject>
#include "../../environment/network_environment/NetworkEdge.h"
#include "../../skill/move/MoveSkill.h"

class MoveThroughNetworkSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughNetworkSkill();
    ~MoveThroughNetworkSkill();

    //- PROPERTIES
    static QString EDGE_CAPACITY_PROP;
    static QString EDGE_INSIDE_AGENT_IDS_PROP;
    static QString CURRENT_ROAD_ID;
    static QString CURRENT_ROAD_TYPE;
    static QString CURRENT_ROAD_MAXSPEED;
    static QString STORE_ROUTE_AS;

    // GETTERS
    QSharedPointer<GWSEntity> getCurrentEdge() const;
    GWSGeometry getCurrentMovingTowards() const;

    // METHODS
    virtual void move( const GWSTimeUnit &movement_duration , const GWSSpeedUnit &movement_speed , GWSGeometry &route_destination_geom , QString &graph_type );

protected:

    QList< QSharedPointer<GWSEntity> > pending_route_edges;
    QList< GWSCoordinate > pending_edge_coordinates;
    GWSCoordinate last_route_started_from;

};

#endif // MOVETHROUGHNETWORKSKILL_H
