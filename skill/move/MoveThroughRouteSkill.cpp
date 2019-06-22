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

GWSGeometry MoveThroughRouteSkill::getCurrentMovingTowards() const{

    if( !this->pending_edge_coordinates.isEmpty() ){
        GWSCoordinate c = this->pending_edge_coordinates.at( 0 );
        return GWSGeometry( c );
    }
    return GWSGeometry();
}


/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughRouteSkill::move(const GWSTimeUnit &movement_duration, const GWSSpeedUnit &movement_speed, GWSGeometry &route_destination_geom, QString &graph_type){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<GWSEntity> agent = this->getEntity();
    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Reset travelling road
    agent->setProperty( CURRENT_ROAD_ID , QJsonValue::Undefined );
    agent->setProperty( CURRENT_ROAD_TYPE , QJsonValue::Undefined );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , QJsonValue::Undefined );
    agent->setProperty( STORE_ROUTE_AS , QJsonValue::Undefined );

    if( !agent_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getUID() );
        return;
    }

    if( agent_geom == route_destination_geom ){
        return;
    }

    GWSGeometry move_to = GWSGeometry( route_destination_geom );

    // Extract destination coordinates
    GWSCoordinate current_coor = agent_geom.getCentroid();
    GWSCoordinate route_destination_coor = route_destination_geom.getCentroid();

    if( this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ){

        // Generate pending route
        if( graph_type.isEmpty() ){ graph_type = GWSEntity::staticMetaObject.className(); }
        QStringList agent_ids = GWSNetworkEnvironment::globalInstance()->getShortestPath( current_coor , route_destination_coor , graph_type );

        if( !agent_ids.isEmpty() ){
            this->pending_route_edges = GWSEntityEnvironment::globalInstance()->getByUIDS( agent_ids );
        }

        // If calculated a route whose first point was already calculated in a previous call to this method,
        // it means we are running in circles
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
        MoveSkill::move( qMin( pending_time , movement_duration ), movement_speed , move_to );
        return;
    }

    // If pending_route_edges is not empty, we are moving through the pos(0) edge
    QSharedPointer<GWSEntity> current_edge_agent = this->getCurrentEdge();
    agent->setProperty( CURRENT_ROAD_ID , current_edge_agent->getUID() );
    agent->setProperty( CURRENT_ROAD_TYPE , current_edge_agent->getProperty( "type" ) );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , current_edge_agent->getProperty( MoveSkill::MAX_SPEED ) );

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        move_to = GWSGeometry( this->pending_edge_coordinates.at( 0 ) );

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        if( agent_geom.getDistance( move_to ) < GWSLengthUnit( 0.5 ) ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        if( this->pending_edge_coordinates.isEmpty() ){
            // Have completed the edge coordinates, so remove the edge too
            this->pending_route_edges.removeAt( 0 );
        }

        // Continue to next if
    }

    if( !this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        GWSGeometry current_edge_agent_geometry = GWSGeometry( current_edge_agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        current_edge_agent_geometry = GWSGeometryTransformators::transformSimplify( current_edge_agent_geometry , 0.0001 );
        this->pending_edge_coordinates = GWSGeometryGetters::getCoordinates( current_edge_agent_geometry );

        // Set destination to next coordinate
        if ( !this->pending_edge_coordinates.isEmpty() ){

            if( current_coor == this->pending_edge_coordinates.at( 0 ) ){
                this->pending_edge_coordinates.removeAt( 0 );
            }

            move_to = GWSGeometry( this->pending_edge_coordinates.at( 0 ) );
        }
    }

    MoveSkill::move( movement_duration , movement_speed , move_to );
}
