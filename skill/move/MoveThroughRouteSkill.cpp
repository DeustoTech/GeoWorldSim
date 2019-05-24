#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../app/App.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../util/geometry/GeometryTransformators.h"

QString MoveThroughRouteSkill::EDGE_CAPACITY_PROP = "capacity";
QString MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP = "agents_inside_edge_ids";
QString MoveThroughRouteSkill::CURRENT_ROAD_ID = "current_road_id";
QString MoveThroughRouteSkill::CURRENT_ROAD_TYPE = "current_road_type";
QString MoveThroughRouteSkill::CURRENT_ROAD_MAXSPEED = "current_road_maxspeed";
QString MoveThroughRouteSkill::STORE_ROUTE_AS = "calculated_route";

MoveThroughRouteSkill::MoveThroughRouteSkill() : MoveSkill(){
    //this->setProperty( SKILL_NETWORK_TYPE_PROP , "GWSAgent" );
}

MoveThroughRouteSkill::~MoveThroughRouteSkill(){
}


/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<GWSAgent> MoveThroughRouteSkill::getCurrentEdge() const{
    if( !this->pending_route_edges.isEmpty() ){
        return this->pending_route_edges.at( 0 );
    }
    return Q_NULLPTR;
}

GWSCoordinate MoveThroughRouteSkill::getCurrentMonvintTowards() const{
    if( !this->pending_edge_coordinates.isEmpty() ){
        return  this->pending_edge_coordinates.at( 0 );
    }
    return GWSCoordinate();
}


/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate route_destination , QString graph_type ){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Reset travelling road
    agent->setProperty( CURRENT_ROAD_ID , QJsonValue() );
    agent->setProperty( CURRENT_ROAD_TYPE , QJsonValue() );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , QJsonValue() );
    agent->setProperty( STORE_ROUTE_AS , QJsonValue() );

    if( !agent_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getUID() );
        return;
    }

    // Extract destination coordinates
    GWSCoordinate current_coor = agent_geom.getCentroid();

    if( current_coor == route_destination ){
        return;
    }

    if( this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ){

        // Generate pending route
        if( graph_type.isEmpty() ){ graph_type = GWSAgent::staticMetaObject.className(); }
        QStringList agent_ids = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , route_destination , graph_type );

        if( !agent_ids.isEmpty() ){
            this->pending_route_edges = GWSAgentEnvironment::globalInstance()->getByUIDS( agent_ids );
        }

        // If calculated a route whose first point is even further than the destination, don't follow the route
        // Avoids recalculating a route once finished routing and freeflowing to destination
        if( !this->pending_route_edges.isEmpty() ){

            GWSCoordinate new_route_start = GWSNetworkEdge( this->pending_route_edges.at( 0 )->getProperty( GWSNetworkEnvironment::EDGE_PROP ).toObject() ).getFromCoordinate();
            if( new_route_start == this->last_route_started_from ){
                this->pending_route_edges.clear();
            } else {
                this->last_route_started_from = new_route_start;
            }

            // Store route ONLY WHEN CALCULATED, afterwards it will be removed
            QJsonObject geojson;
            geojson.insert( "type" , "LineString" );
            QJsonArray coordinates;
            foreach (QSharedPointer<GWSAgent> edge , this->pending_route_edges) {
                GWSGeometry edge_geom = GWSGeometry( edge->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
                foreach( GWSCoordinate c , GWSGeometryGetters::getCoordinates( edge_geom ) ){
                    coordinates.append( QJsonArray( { c.getX() , c.getY() , c.getZ() } ) );
                }
            }

            geojson.insert( "coordinates" , coordinates );
            agent->setProperty( STORE_ROUTE_AS , geojson );

        }
    }

    // Assume we have reached route end OR not found route, move freely
    if( this->pending_route_edges.isEmpty() ){

        // Pending time to reach destination can be higher than the duration requested.
        GWSLengthUnit pending_distance = current_coor.getDistance( route_destination );
        GWSTimeUnit pending_time = pending_distance.number() / movement_speed.number(); // Time needed to reach route_destination at current speed
        MoveSkill::move( qMin( pending_time , movement_duration ), movement_speed , route_destination );
        return;
    }

    // If pending_route_edges is not empty
    QSharedPointer<GWSAgent> current_edge_agent = this->getCurrentEdge();
    agent->setProperty( CURRENT_ROAD_ID , current_edge_agent->getUID() );
    agent->setProperty( CURRENT_ROAD_TYPE , current_edge_agent->getProperty( "type" ) );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , current_edge_agent->getProperty( "maxspeed" ) );

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSCoordinate next_coordinate = this->pending_edge_coordinates.at( 0 );
        if( current_coor == next_coordinate ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        GWSCoordinate move_to;
        if( this->pending_edge_coordinates.isEmpty() ){

            // Have completed the edge coordinates, so remove the edge too (if exists)
            this->pending_route_edges.removeAt( 0 );
            move_to = current_coor;

        } else {
            //move_to = this->pending_edge_coordinates.at( 0 );
            move_to = next_coordinate;
        }

        route_destination = move_to;
    }

    if( !this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        // First look if edge has a capacity and therefore we can enter edge
        double edge_capacity = -1;
        if( !current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).isNull() ){
            edge_capacity = current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).toDouble();
        }

        if( edge_capacity >= 0 ){
            int edge_inside_agents_amount = current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP ).toArray().size();
            if( edge_capacity <= edge_inside_agents_amount ){
                // Wait for edge to liberate, that is, do not move
                MoveSkill::move( 0 , GWSSpeedUnit( 0 ) , route_destination );
                return;
            }
        }

        // We can enter, so increment or maintain speed
        //movement_speed = qMin( starting_current_edge_max_speed.number() , movement_speed.number() + 10 );

        GWSGeometry current_edge_agent_geometry = GWSGeometry( current_edge_agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        current_edge_agent_geometry = GWSGeometryTransformators::transformSimplify( current_edge_agent_geometry , 0.05 );
        this->pending_edge_coordinates = GWSGeometryGetters::getCoordinates( current_edge_agent_geometry );

        // Set destination to next coordinate
        if ( !this->pending_edge_coordinates.isEmpty() ){
            if( route_destination == this->pending_edge_coordinates.at( 0 ) ){
                this->pending_edge_coordinates.removeAt( 0 );
            }
            route_destination = this->pending_edge_coordinates.at( 0 );
        }

    }

    MoveSkill::move( movement_duration , movement_speed , route_destination );
}
