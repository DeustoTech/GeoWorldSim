#ifndef MOVETHROUGHNETWORKSKILL_H
#define MOVETHROUGHNETWORKSKILL_H

#include <QObject>
#include "../../environment/network_environment/NetworkEdge.h"
#include "../../skill/move/MoveSkill.h"

namespace geoworldsim {
namespace skill {

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
    QSharedPointer<Entity> getCurrentEdge() const;
    geometry::Geometry getCurrentMovingTowards() const;

    // METHODS
    virtual void move( const unit::TimeUnit &movement_duration , const unit::SpeedUnit &movement_speed , geometry::Geometry &route_destination_geom , QString &graph_type );

protected:

    QList< QSharedPointer<Entity> > pending_route_edges;
    QList< geometry::Coordinate > pending_edge_coordinates;
    geometry::Coordinate last_route_started_from;

};

}
}

#endif // MOVETHROUGHNETWORKSKILL_H
