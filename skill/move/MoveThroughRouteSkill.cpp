#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../app/App.h"

QString MoveThroughRouteSkill::SKILL_NETWORK_TYPE_PROP = "network_type";
QString MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_X_PROP = "route_destination_x";
QString MoveThroughRouteSkill::SKILL_ROUTE_DESTINATION_Y_PROP = "route_destination_y";
QString MoveThroughRouteSkill::AGENT_CURRENT_ROAD_ID_PROP = "current_road_id";
QString MoveThroughRouteSkill::AGENT_CURRENT_ROAD_TYPE_PROP = "current_road_type";
QString MoveThroughRouteSkill::AGENT_CURRENT_ROAD_MAXSPEED_PROP = "current_road_maxspeed";

MoveThroughRouteSkill::MoveThroughRouteSkill() : MoveSkill(){
}

MoveThroughRouteSkill::~MoveThroughRouteSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSCoordinate MoveThroughRouteSkill::getRouteDestination() {
    QSharedPointer<GWSAgent> agent = this->getAgent();
    if( this->getProperty( SKILL_ROUTE_DESTINATION_X_PROP ).isNull() || this->getProperty( SKILL_ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( NAN , NAN , NAN );
    }
    return GWSCoordinate( this->getProperty( SKILL_ROUTE_DESTINATION_X_PROP ).toDouble( ) , this->getProperty( SKILL_ROUTE_DESTINATION_Y_PROP ).toDouble( ) , 0 );
}

/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move( GWSTimeUnit movement_duration ){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
        return;
    }

    // Extract destination coordinates
    GWSCoordinate current_coor = agent_geom->getCentroid();
    GWSCoordinate destination_coor = this->getRouteDestination();

    if( current_coor == destination_coor ){
        return;
    }

    if( this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ){
        // Generate pending route
        QString graph_type = this->getProperty( SKILL_NETWORK_TYPE_PROP ).toString();
        if( graph_type.isEmpty() ){ graph_type = GWSAgent::staticMetaObject.className(); }
        this->pending_route = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , destination_coor , graph_type );
    }

    // Assume we have reached route end OR not found route, free move to destination
    if( this->pending_route.isEmpty() ){
        this->setProperty( MoveSkill::SKILL_MOVING_TOWARDS_X_PROP , destination_coor.getX() );
        this->setProperty( MoveSkill::SKILL_MOVING_TOWARDS_Y_PROP , destination_coor.getY() );
        MoveSkill::move( movement_duration );
        return;
    }

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        QSharedPointer<GWSGraphEdge> starting_current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );
        //agent->setProperty( MoveThroughRouteBehaviour::STORE_CURRENT_ROAD_TYPE_AS , starting_current_edge_agent->getProperty( "highway" ) );
        //agent->setProperty( MoveThroughRouteBehaviour::STORE_CURRENT_ROAD_MAXSPEED_AS , starting_current_edge_agent->getProperty( "maxspeed") );

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSCoordinate next_coordinate = this->pending_edge_coordinates.at( 0 );
        if( current_coor == next_coordinate ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        GWSCoordinate move_to;
        if( this->pending_edge_coordinates.isEmpty() ){

            QSharedPointer<GWSGraphEdge> starting_current_edge = this->pending_route.at(0);
            QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

            // Remove road information from agent
            agent->setProperty( AGENT_CURRENT_ROAD_ID_PROP , QVariant() );
            agent->setProperty( AGENT_CURRENT_ROAD_TYPE_PROP , QVariant() );
            agent->setProperty( AGENT_CURRENT_ROAD_MAXSPEED_PROP , QVariant() );

            // Remove agent from road
            QStringList inside_agent_ids = starting_current_edge_agent->getProperty( GWSNetworkEnvironment::INSIDE_AGENT_IDS_PROP ).toStringList();
            inside_agent_ids.removeAll( agent->getId() );
            starting_current_edge_agent->setProperty( GWSNetworkEnvironment::INSIDE_AGENT_IDS_PROP , inside_agent_ids );

            this->pending_route.removeAt( 0 ); // Have completed the edge coordinates, so remove the edge too
            move_to = current_coor;
        } else {
            move_to = this->pending_edge_coordinates.at( 0 );
        }

        this->setProperty( MoveSkill::SKILL_MOVING_TOWARDS_X_PROP , move_to.getX() );
        this->setProperty( MoveSkill::SKILL_MOVING_TOWARDS_Y_PROP , move_to.getY() );
    }

    if( !this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        // We are going to start iterating the coordinates of edge located at pending_route[0]
        QSharedPointer<GWSGraphEdge> starting_current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

        // Store road information in agent
        agent->setProperty( AGENT_CURRENT_ROAD_ID_PROP , starting_current_edge_agent->getId() );
        agent->setProperty( AGENT_CURRENT_ROAD_TYPE_PROP , starting_current_edge_agent->getProperty( "highway") );
        agent->setProperty( AGENT_CURRENT_ROAD_MAXSPEED_PROP , starting_current_edge_agent->getProperty( "maxspeed") );

        // Add agent to road
        QStringList inside_agent_ids = starting_current_edge_agent->getProperty( GWSNetworkEnvironment::INSIDE_AGENT_IDS_PROP ).toStringList();
        inside_agent_ids.append( agent->getId() );
        starting_current_edge_agent->setProperty( GWSNetworkEnvironment::INSIDE_AGENT_IDS_PROP , inside_agent_ids );

        QSharedPointer<GWSGeometry> current_edge_agent_geometry = GWSPhysicalEnvironment::globalInstance()->getGeometry( starting_current_edge_agent );
        this->pending_edge_coordinates = current_edge_agent_geometry->getCoordinates();
    }

    MoveSkill::move( movement_duration );
}
