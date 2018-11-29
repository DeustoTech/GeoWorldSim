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
    //this->setProperty( SKILL_NETWORK_TYPE_PROP , "GWSAgent" );
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
    GWSSpeedUnit destination_speed = agent->getProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP ).toDouble();

    if( current_coor == destination_coor ){
        return;
    }
 //if( this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() && !route_found_borrame )
    if( this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty()  ){
        // Generate pending route
        QString graph_type = this->getProperty( SKILL_NETWORK_TYPE_PROP ).toString();
        if( graph_type.isEmpty() ){ graph_type = GWSAgent::staticMetaObject.className(); }
        this->pending_route = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , destination_coor , graph_type );
        //this->route_found_borrame = true;
    }

    // Assume we have reached route end OR not found route, move freely
    if( this->pending_route.isEmpty() ){
        GWSLengthUnit pending_distance = current_coor.getDistance( destination_coor );
        MoveSkill::move( movement_duration , GWSSpeedUnit( qMin( 4.0 , pending_distance.number() ) ) , destination_coor );
        return;
    }

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        QSharedPointer<GWSGraphEdge> starting_current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSCoordinate next_coordinate = this->pending_edge_coordinates.at( 0 );
        if( current_coor == next_coordinate ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        GWSCoordinate move_to;
        if( this->pending_edge_coordinates.isEmpty() ){

            // Remove road information from agent
            agent->setProperty( AGENT_CURRENT_ROAD_ID_PROP , QJsonValue() );
            agent->setProperty( AGENT_CURRENT_ROAD_TYPE_PROP , QJsonValue() );
            agent->setProperty( AGENT_CURRENT_ROAD_MAXSPEED_PROP , QJsonValue() );

            // Remove agent from road
            QJsonArray inside_agent_ids = starting_current_edge->getProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP ).toArray();
            QJsonArray new_inside_agent_ids;
            foreach (QJsonValue v, inside_agent_ids ) {
                if( v != agent->getId() ){ new_inside_agent_ids.append( v ); }
            }
            starting_current_edge->setProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP , new_inside_agent_ids );

            // Have completed the edge coordinates, so remove the edge too (if exists)
            this->pending_route.removeAt( 0 );
            move_to = current_coor;

        } else {
            //move_to = this->pending_edge_coordinates.at( 0 );
            move_to = next_coordinate;
        }

        destination_coor = move_to;
        destination_speed = qMin( starting_current_edge_agent->getProperty( "maxspeed" ).toDouble() , destination_speed.number()+ 10 );
    }

    if( !this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        // We are going to start iterating the coordinates of edge located at pending_route[0]
        QSharedPointer<GWSGraphEdge> starting_current_edge = this->pending_route.at(0);

        QSharedPointer<GWSAgent> starting_current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( starting_current_edge );

        GWSSpeedUnit starting_current_edge_max_speed = starting_current_edge_agent->getProperty( "maxspeed").toDouble();

        // First look if edge has a capacity and therefore we can enter edge
        double edge_capacity = -1;
        if( !starting_current_edge->getProperty( GWSNetworkEnvironment::EDGE_CAPACITY_PROP ).isNull() ){
            edge_capacity = starting_current_edge->getProperty( GWSNetworkEnvironment::EDGE_CAPACITY_PROP ).toDouble();
        }

        if( edge_capacity >= 0 ){
            int edge_inside_agents_amount = starting_current_edge->getProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP ).toArray().size();
            if( edge_capacity <= edge_inside_agents_amount ){
                // Wait for edge to liberate, that is, do not move
                MoveSkill::move( 0 , GWSSpeedUnit( 0 ) , destination_coor );
                return;
            }
        }

        // We can enter, so increment or maintain speed
        destination_speed = qMin( starting_current_edge_max_speed.number() , destination_speed.number() + 10 );

        // Store road information in agent
        agent->setProperty( AGENT_CURRENT_ROAD_ID_PROP , starting_current_edge_agent->getId() );
        agent->setProperty( AGENT_CURRENT_ROAD_TYPE_PROP , starting_current_edge_agent->getProperty( "highway") );
        agent->setProperty( AGENT_CURRENT_ROAD_MAXSPEED_PROP , starting_current_edge_max_speed );

        // Add agent to road
        QJsonArray inside_agent_ids = starting_current_edge->getProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP ).toArray();
        inside_agent_ids.append( agent->getId() );
        starting_current_edge->setProperty( GWSNetworkEnvironment::EDGE_INSIDE_AGENT_IDS_PROP , inside_agent_ids );

        QSharedPointer<GWSGeometry> current_edge_agent_geometry = GWSPhysicalEnvironment::globalInstance()->getGeometry( starting_current_edge_agent );
        this->pending_edge_coordinates = current_edge_agent_geometry->getCoordinates();
    }

    MoveSkill::move( movement_duration , destination_speed , destination_coor );
    
}
