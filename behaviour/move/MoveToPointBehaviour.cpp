#include "MoveToPointBehaviour.h"

#include "../../app/App.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString geoworldsim::behaviour::MoveToPointBehaviour::INPUT_TRANSPORT_NETWORK_TYPE = "input_transport_network_type";
QString geoworldsim::behaviour::MoveToPointBehaviour::INPUT_ROUTE_DESTINATION = "input_route_destination";
QString geoworldsim::behaviour::MoveToPointBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString geoworldsim::behaviour::MoveToPointBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";

QString geoworldsim::behaviour::MoveToPointBehaviour::CURRENT_ROAD_ID = "current_road_id";
QString geoworldsim::behaviour::MoveToPointBehaviour::CURRENT_ROAD_TYPE = "current_road_type";
QString geoworldsim::behaviour::MoveToPointBehaviour::CURRENT_ROAD_MAXSPEED = "current_road_maxspeed";
QString geoworldsim::behaviour::MoveToPointBehaviour::STORE_ROUTE_AS = "calculated_route";

QString geoworldsim::behaviour::MoveToPointBehaviour::INSTANT_DISTANCE = "instant_travelled_distance";
QString geoworldsim::behaviour::MoveToPointBehaviour::ACCUMULATED_DISTANCE = "accumulated_travelled_distance";
QString geoworldsim::behaviour::MoveToPointBehaviour::INSTANT_TIME = "instant_travelled_time";
QString geoworldsim::behaviour::MoveToPointBehaviour::ACCUMULATED_TIME = "accumulated_travelled_time";


geoworldsim::behaviour::MoveToPointBehaviour::MoveToPointBehaviour() : Behaviour(){
}

geoworldsim::behaviour::MoveToPointBehaviour::~MoveToPointBehaviour(){
}

/**********************************************************************
 METHODS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::MoveToPointBehaviour::behave(){

    QSharedPointer<Entity> entity = this->getEntity();
    geoworldsim::geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    unit::TimeUnit duration_of_movement = this->getProperty( BEHAVIOUR_DURATION ).toDouble( 1 );

    geometry::Geometry route_destination = this->getProperty( INPUT_ROUTE_DESTINATION ).toObject();
    if( !route_destination.isValid() ){
        qWarning() << QString("Entity %1 %2 has invalid destination to route to").arg( entity->metaObject()->className() ).arg( entity->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    entity->setProperty( environment::PhysicalEnvironment::GEOMETRY_PROP , route_destination.getGeoJSON() );

    entity->setProperty( INSTANT_TIME , duration_of_movement.number() );
    entity->setProperty( ACCUMULATED_TIME , entity->getProperty( ACCUMULATED_TIME ).toDouble() + duration_of_movement.number() );

    // Once moved, check if arrived and set NEXTS behaviour
    geometry::Geometry entity_geom_post = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    if ( entity_geom_post.getDistance( route_destination ) < unit::LengthUnit( 0.5 ) ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    // Se MUEVE
    return QPair< double , QJsonArray >( duration_of_movement.number() ,  this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
}
