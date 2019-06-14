#include "MoveThroughRouteSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
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

QSharedPointer<GWSEntity> MoveThroughRouteSkill::getCurrentEdge() const{
    if( !this->pending_route_edges.isEmpty() ){
        return this->pending_route_edges.at( 0 );
    }
    return Q_NULLPTR;
}

GWSCoordinate MoveThroughRouteSkill::getCurrentMonvintTowards() const{
    if( !this->pending_edge_geometries.isEmpty() ){
        return  this->pending_edge_geometries.at( 0 );
    }
    return GWSCoordinate();
}


/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSGeometry route_destination_geom , QString graph_type ){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSEntity> agent = this->getEntity();
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
    GWSCoordinate route_destination_coor = route_destination_geom.getCentroid();

    if( agent_geom == route_destination_geom ){
        return;
    }

    if( this->pending_route_edges.isEmpty() && this->pending_edge_geometries.isEmpty() ){

        // Generate pending route
        if( graph_type.isEmpty() ){ graph_type = GWSEntity::staticMetaObject.className(); }
        QStringList agent_ids = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , route_destination_coor , graph_type );

        if( !agent_ids.isEmpty() ){
            this->pending_route_edges = GWSEntityEnvironment::globalInstance()->getByUIDS( agent_ids );
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
            foreach (QSharedPointer<GWSEntity> edge , this->pending_route_edges) {
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
        GWSLengthUnit pending_distance = current_coor.getDistance( route_destination_coor );
        GWSTimeUnit pending_time = pending_distance.number() / movement_speed.number(); // Time needed to reach route_destination at current speed
        MoveSkill::move( qMin( pending_time , movement_duration ), movement_speed , route_destination_geom );
        return;
    }

    // If pending_route_edges is not empty
    QSharedPointer<GWSEntity> current_edge_agent = this->getCurrentEdge();
    agent->setProperty( CURRENT_ROAD_ID , current_edge_agent->getUID() );
    agent->setProperty( CURRENT_ROAD_TYPE , current_edge_agent->getProperty( "type" ) );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , current_edge_agent->getProperty( "maxspeed" ) );

    // Continue following coordinates
    if ( !this->pending_edge_geometries.isEmpty() ){

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        GWSGeometry next_coordinate_geom = this->pending_edge_geometries.at( 0 );
        GWSCoordinate next_coor = next_coordinate_geom.getCentroid();
        if( current_coor.getDistance( next_coor ) < GWSLengthUnit( 0.5 ) ){
            this->pending_edge_geometries.removeAt( 0 );
        }

        GWSGeometry move_to;
        if( this->pending_edge_geometries.isEmpty() ){

            // Have completed the edge coordinates, so remove the edge too (if exists)
            this->pending_route_edges.removeAt( 0 );
            move_to = agent_geom;

        } else {
            //move_to = this->pending_edge_coordinates.at( 0 );
            move_to = next_coordinate_geom;
        }

        route_destination_geom = move_to;
    }

    if( !this->pending_route_edges.isEmpty() && this->pending_edge_geometries.isEmpty() ) {

        // First look if edge has a capacity and therefore we can enter edge
        double edge_capacity = -1;
        if( !current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).isNull() ){
            edge_capacity = current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_CAPACITY_PROP ).toDouble();
        }

        if( edge_capacity >= 0 ){
            int edge_inside_agents_amount = current_edge_agent->getProperty( MoveThroughRouteSkill::EDGE_INSIDE_AGENT_IDS_PROP ).toArray().size();
            if( edge_capacity <= edge_inside_agents_amount ){
                // Wait for edge to liberate, that is, do not move
                MoveSkill::move( 0 , GWSSpeedUnit( 0 ) , route_destination_geom );
                return;
            }
        }

        // We can enter, so increment or maintain speed
        //movement_speed = qMin( starting_current_edge_max_speed.number() , movement_speed.number() + 10 );

        GWSGeometry current_edge_agent_geometry = GWSGeometry( current_edge_agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        current_edge_agent_geometry = GWSGeometryTransformators::transformSimplify( current_edge_agent_geometry , 0.005 );
        this->pending_edge_geometries = GWSGeometryGetters::getCoordinates( current_edge_agent_geometry );

        // Set destination to next coordinate
        if ( !this->pending_edge_geometries.isEmpty() ){
            if( route_destination_coor == this->pending_edge_geometries.at( 0 ) ){
                this->pending_edge_geometries.removeAt( 0 );
            }
            route_destination_coor = this->pending_edge_geometries.at( 0 );
        }

    }

    MoveSkill::move( movement_duration , movement_speed , route_destination_geom );
}
