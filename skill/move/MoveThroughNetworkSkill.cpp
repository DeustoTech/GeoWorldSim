#include "MoveThroughNetworkSkill.h"
#include "../../environment/Environment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../app/App.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../util/geometry/GeometryTransformators.h"

QString geoworldsim::skill::MoveThroughNetworkSkill::EDGE_CAPACITY_PROP = "capacity";
QString geoworldsim::skill::MoveThroughNetworkSkill::EDGE_INSIDE_AGENT_IDS_PROP = "agents_inside_edge_ids";
QString geoworldsim::skill::MoveThroughNetworkSkill::CURRENT_ROAD_ID = "current_road_id";
QString geoworldsim::skill::MoveThroughNetworkSkill::CURRENT_ROAD_TYPE = "current_road_type";
QString geoworldsim::skill::MoveThroughNetworkSkill::CURRENT_ROAD_MAXSPEED = "current_road_maxspeed";
QString geoworldsim::skill::MoveThroughNetworkSkill::STORE_ROUTE_AS = "calculated_route";

using namespace geoworldsim;

geoworldsim::skill::MoveThroughNetworkSkill::MoveThroughNetworkSkill() : MoveSkill(){
    //this->setProperty( SKILL_NETWORK_TYPE_PROP , "GWSAgent" );
}

geoworldsim::skill::MoveThroughNetworkSkill::~MoveThroughNetworkSkill(){
}


/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<Entity> geoworldsim::skill::MoveThroughNetworkSkill::getCurrentEdge() const{
    if( !this->pending_route_edges.isEmpty() ){
        return this->pending_route_edges.at( 0 );
    }
    return Q_NULLPTR;
}

geometry::Geometry geoworldsim::skill::MoveThroughNetworkSkill::getCurrentMovingTowards() const{

    if( !this->pending_edge_coordinates.isEmpty() ){
        geometry::Coordinate c = this->pending_edge_coordinates.at( 0 );
        return geometry::Geometry( c );
    }
    return geometry::Geometry();
}


/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::skill::MoveThroughNetworkSkill::move(const unit::TimeUnit &movement_duration, const unit::SpeedUnit &movement_speed, geometry::Geometry &route_destination_geom, QString &graph_type){

    // Extract current coordinates of Skilled GWSAgent
    QSharedPointer<Entity> agent = this->getEntity();
    geometry::Geometry agent_geom = geometry::Geometry( agent->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Reset travelling road
    agent->setProperty( CURRENT_ROAD_ID , QJsonValue::Null );
    agent->setProperty( CURRENT_ROAD_TYPE , QJsonValue::Null );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , QJsonValue::Null );
    agent->setProperty( STORE_ROUTE_AS , QJsonValue::Null );

    if( !agent_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getUID() );
        return;
    }

    if( agent_geom == route_destination_geom ){
        return;
    }

    geometry::Geometry move_to = geometry::Geometry( route_destination_geom );

    // Extract destination coordinates
    geometry::Coordinate current_coor = agent_geom.getCentroid();
    geometry::Coordinate route_destination_coor = route_destination_geom.getCentroid();

    if( this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ){

        // Generate pending route
        if( graph_type.isEmpty() ){ graph_type = Entity::staticMetaObject.className(); }
        QStringList agent_ids = environment::NetworkEnvironment::globalInstance()->getShortestPath( current_coor , route_destination_coor , graph_type );

        if( !agent_ids.isEmpty() ){
            this->pending_route_edges = environment::EntityEnvironment::globalInstance()->getByUIDS( agent_ids );
        }

        // If calculated a route whose first point was already calculated in a previous call to this method,
        // it means we are running in circles
        if( !this->pending_route_edges.isEmpty() ){

           geometry::Coordinate new_route_start = graph::NetworkEdge( this->pending_route_edges.at( 0 )->getProperty( environment::NetworkEnvironment::EDGE_PROP ).toObject() ).getFromCoordinate();
           if( new_route_start == this->last_route_started_from ){
                this->pending_route_edges.clear();
            } else {
                this->last_route_started_from = new_route_start;
            }

            // Store route ONLY WHEN CALCULATED, afterwards it will be removed
            QJsonObject geojson;
            geojson.insert( "type" , "LineString" );
            QJsonArray coordinates;
            foreach (QSharedPointer<Entity> edge , this->pending_route_edges) {
                geometry::Geometry edge_geom = geometry::Geometry( edge->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
                foreach( geometry::Coordinate c , geometry::GeometryGetters::getCoordinates( edge_geom ) ){
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
        unit::LengthUnit pending_distance = current_coor.getDistance( route_destination_coor );
        unit::TimeUnit pending_time = pending_distance.number() / movement_speed.number(); // Time needed to reach route_destination at current speed
        MoveSkill::move( qMin( pending_time , movement_duration ), movement_speed , move_to );
        return;
    }

    // If pending_route_edges is not empty, we are moving through the pos(0) edge
    QSharedPointer<Entity> current_edge_agent = this->getCurrentEdge();
    agent->setProperty( CURRENT_ROAD_ID , current_edge_agent->getUID() );
    agent->setProperty( CURRENT_ROAD_TYPE , current_edge_agent->getProperty( "type" ) );
    agent->setProperty( CURRENT_ROAD_MAXSPEED , current_edge_agent->getProperty( MoveSkill::MAX_SPEED ) );

    // Continue following coordinates
    if ( !this->pending_edge_coordinates.isEmpty() ){

        move_to = geometry::Geometry( this->pending_edge_coordinates.at( 0 ) );

        // Get next real edge geometry's coordinate (not the ones from the edge), and move to them
        if( agent_geom.getDistance( move_to ) < unit::LengthUnit( 0.5 ) ){
            this->pending_edge_coordinates.removeAt( 0 );
        }

        if( this->pending_edge_coordinates.isEmpty() ){
            // Have completed the edge coordinates, so remove the edge too
            this->pending_route_edges.removeAt( 0 );
        }

        // Continue to next if
    }

    if( !this->pending_route_edges.isEmpty() && this->pending_edge_coordinates.isEmpty() ) {

        geometry::Geometry current_edge_agent_geometry = geometry::Geometry( current_edge_agent->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        current_edge_agent_geometry = geometry::GeometryTransformators::transformSimplify( current_edge_agent_geometry , 0.0001 );
        this->pending_edge_coordinates = geometry::GeometryGetters::getCoordinates( current_edge_agent_geometry );

        // Set destination to next coordinate
        if ( !this->pending_edge_coordinates.isEmpty() ){

            if( current_coor == this->pending_edge_coordinates.at( 0 ) ){
                this->pending_edge_coordinates.removeAt( 0 );
            }

            move_to = geometry::Geometry( this->pending_edge_coordinates.at( 0 ) );
        }
    }

    MoveSkill::move( movement_duration , movement_speed , move_to );
}
