#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../app/App.h"
#include "../../skill/pollute/PolluteSkill.h"
#include "../../util/geometry/GeometryGetters.h"

QString MoveThroughRouteSkill::EDGE_CAPACITY_PROP = "capacity";
QString MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP = "agents_inside_edge_ids";

QString MoveThroughRouteSkill::STORE_CURRENT_ROAD_ID = "current_road_id";
QString MoveThroughRouteSkill::STORE_CURRENT_ROAD_TYPE = "current_road_type";
QString MoveThroughRouteSkill::STORE_CURRENT_ROAD_MAXSPEED = "current_road_maxspeed";

MoveThroughRouteSkill::MoveThroughRouteSkill() : MoveSkill(){
    //this->setProperty( SKILL_NETWORK_TYPE_PROP , "GWSAgent" );
}

MoveThroughRouteSkill::~MoveThroughRouteSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/*GWSCoordinate MoveThroughRouteSkill::getRouteDestination() {
    QSharedPointer<GWSAgent> agent = this->getAgent();
    if( agent->getProperty( AGENT_ROUTE_DESTINATION_X_PROP ).isNull() || agent->getProperty( AGENT_ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( NAN , NAN , NAN );
    }
    return GWSCoordinate( agent->getProperty( AGENT_ROUTE_DESTINATION_X_PROP ).toDouble( ) , agent->getProperty( AGENT_ROUTE_DESTINATION_Y_PROP ).toDouble( ) , 0 );
}*/

/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate route_destination , QString graph_type ){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSGeometry agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getUID() );
        return;
    }

    // Extract destination coordinates
    GWSCoordinate current_coor = agent_geom.getCentroid();

    if( current_coor == route_destination ){
        return;
    }


    if( this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ){

        // Generate pending route
        if( graph_type.isEmpty() ){ graph_type = GWSAgent::staticMetaObject.className(); }
        this->pending_route = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , route_destination , graph_type );

        // If calculated a route whose first point is even further than the destination, don¡t follow the route
        // Avoids recalculating a route once finished routing and freeflowing to destination
        if( !this->pending_route.isEmpty() ){
            GWSCoordinate new_route_start = this->pending_route.at( 0 ).getFromCoordinate();
            if( new_route_start == this->last_route_started_from ){
                this->pending_route.clear();
            } else {
                this->last_route_started_from = new_route_start;
            }
        }
    }

    // Assume we have reached route end OR not found route, move freely
    if( this->pending_route.isEmpty() ){
        GWSLengthUnit pending_distance = current_coor.getDistance( route_destination );
        MoveSkill::move( movement_duration , GWSSpeedUnit( qMin( 4.0 , pending_distance.number() ) ) , route_destination );
        return;
    }

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        GWSNetworkEdge starting_current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSCoordinate next_coordinate = this->pending_edge_coordinates.at( 0 );
        if( current_coor == next_coordinate ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        GWSCoordinate move_to;
        if( this->pending_edge_coordinates.isEmpty() ){

            // Remove road information from agent
            agent->setProperty( STORE_CURRENT_ROAD_ID , QJsonValue() );
            agent->setProperty( STORE_CURRENT_ROAD_TYPE , QJsonValue() );
            agent->setProperty( STORE_CURRENT_ROAD_MAXSPEED , QJsonValue() );

            // Remove agent from road
            QJsonArray inside_agent_ids = starting_current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP ).toArray();
            QJsonArray new_inside_agent_ids;
            foreach (QJsonValue v, inside_agent_ids ) {
                if( v != agent->getUID() ){ new_inside_agent_ids.append( v ); }
            }
            starting_current_edge_agent->setProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP , new_inside_agent_ids );
            // Have completed the edge coordinates, so remove the edge too (if exists)
            this->pending_route.removeAt( 0 );
            move_to = current_coor;

        } else {
            //move_to = this->pending_edge_coordinates.at( 0 );
            move_to = next_coordinate;
        }

        route_destination = move_to;
        movement_speed = qMin( starting_current_edge_agent->getProperty( "maxspeed" ).toDouble() , movement_speed.number()+ 10 );
    }

    if( !this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        // We are going to start iterating the coordinates of edge located at pending_route[0]
        GWSNetworkEdge starting_current_edge = this->pending_route.at(0);

        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

        GWSSpeedUnit starting_current_edge_max_speed = starting_current_edge_agent->getProperty( "maxspeed").toDouble();

        // First look if edge has a capacity and therefore we can enter edge
        double edge_capacity = -1;
        if( !starting_current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).isNull() ){
            edge_capacity = starting_current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).toDouble();
        }

        if( edge_capacity >= 0 ){
            int edge_inside_agents_amount = starting_current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP ).toArray().size();
            if( edge_capacity <= edge_inside_agents_amount ){
                // Wait for edge to liberate, that is, do not move
                MoveSkill::move( 0 , GWSSpeedUnit( 0 ) , route_destination );
                return;
            }
        }

        // We can enter, so increment or maintain speed
        movement_speed = qMin( starting_current_edge_max_speed.number() , movement_speed.number() + 10 );

        // Store road infoVehiclermation in agent
        agent->setProperty( STORE_CURRENT_ROAD_ID , starting_current_edge_agent->getUID() );
        agent->setProperty( STORE_CURRENT_ROAD_TYPE , starting_current_edge_agent->getProperty( "highway") );
        agent->setProperty( STORE_CURRENT_ROAD_MAXSPEED , starting_current_edge_max_speed );

        // Add agent to road
        QJsonArray inside_agent_ids = starting_current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP ).toArray();
        inside_agent_ids.append( agent->getUID() );
        starting_current_edge_agent->setProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP , inside_agent_ids );
        GWSGeometry current_edge_agent_geometry = GWSPhysicalEnvironment::globalInstance()->getGeometry( starting_current_edge_agent );
        this->pending_edge_coordinates = GWSGeometryGetters::getCoordinates( current_edge_agent_geometry );
    }

    MoveSkill::move( movement_duration , movement_speed , route_destination );

}
