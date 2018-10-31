#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../app/App.h"

QString MoveThroughRouteSkill::NETWORK_CLASS_PROP = "network_type";
QString MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP = "route_destination_x";
QString MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP = "route_destination_y";
QString MoveThroughRouteSkill::AGENT_INSIDE_EDGE_PROP = "agent_ids_inside_edge";

MoveThroughRouteSkill::MoveThroughRouteSkill() : MoveSkill(){
    this->setProperty( NETWORK_CLASS_PROP , "GWSAgent" );
}

MoveThroughRouteSkill::~MoveThroughRouteSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

// Generate graph, give me all the agents in the network environment, and generate the graph with them

GWSCoordinate MoveThroughRouteSkill::getRouteDestination() const{
    if( this->getProperty( ROUTE_DESTINATION_X_PROP ).isNull() || this->getProperty( ROUTE_DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( Q_INFINITY , Q_INFINITY , Q_INFINITY );
    }
    return GWSCoordinate( this->getProperty( ROUTE_DESTINATION_X_PROP ) .toDouble( ) , this->getProperty( ROUTE_DESTINATION_Y_PROP ).toDouble( ) , 0 );
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
        QString graph_type = this->getProperty( MoveThroughRouteSkill::NETWORK_CLASS_PROP ).toString();
        this->pending_route = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , destination_coor , graph_type );
    }

    // Assume we have reached route end OR not found route, free move to destination
    if( this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ){
        this->setProperty( MoveSkill::DESTINATION_X_PROP , destination_coor.getX() );
        this->setProperty( MoveSkill::DESTINATION_Y_PROP , destination_coor.getY() );
    }

    if( !this->pending_route.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        // We have reached current_edge's geometry last real coordinate (which should be equal to the edge's "getTo" coordinate)

        // Get next pending_route edge, to get its coordinates and move along them
        QSharedPointer<GWSGraphEdge> current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( current_edge );
        QSharedPointer<GWSGeometry> current_edge_agent_geometry = GWSPhysicalEnvironment::globalInstance()->getGeometry( current_edge_agent );
        this->pending_edge_coordinates = current_edge_agent_geometry->getCoordinates();
        this->pending_route.removeAt( 0 );
        qDebug() << current_edge;
        QStringList list = current_edge_agent->getProperty( AGENT_INSIDE_EDGE_PROP ).toStringList();
        list.removeAll( agent->getId() );
        current_edge_agent->setProperty( AGENT_INSIDE_EDGE_PROP , list );
        qDebug() << list;
    }

    if ( !this->pending_edge_coordinates.isEmpty() ) {

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSCoordinate check_if_arrived = this->pending_edge_coordinates.at( 0 );
        if( current_coor == check_if_arrived ){
            this->pending_edge_coordinates.removeAt( 0 );
        }
        GWSCoordinate move_to;
        if( this->pending_edge_coordinates.isEmpty() ){
            move_to = current_coor;
        } else {
            move_to = this->pending_edge_coordinates.at( 0 );
        }
        this->setProperty( MoveSkill::DESTINATION_X_PROP , move_to.getX() );
        this->setProperty( MoveSkill::DESTINATION_Y_PROP , move_to.getY() );

        QSharedPointer<GWSGraphEdge> current_edge = this->pending_route.at(0);
        QSharedPointer<GWSAgent> current_edge_agent = GWSNetworkEnvironment::globalInstance()->getAgent( current_edge );
        QStringList list = current_edge_agent->getProperty( AGENT_INSIDE_EDGE_PROP ).toStringList();
        if( !list.contains( agent->getId() ) ){
            list.append( agent->getId() );
            current_edge_agent->setProperty( AGENT_INSIDE_EDGE_PROP , list );
        }
    }



    MoveSkill::move( movement_duration );

}
