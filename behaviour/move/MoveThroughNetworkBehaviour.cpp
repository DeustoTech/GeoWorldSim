#include "MoveThroughNetworkBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughNetworkSkill.h"

QString geoworldsim::behaviour::MoveThroughNetworkBehaviour::INPUT_TRANSPORT_NETWORK_TYPE = "input_transport_network_type";
QString geoworldsim::behaviour::MoveThroughNetworkBehaviour::INPUT_ROUTE_DESTINATION = "input_route_destination";
QString geoworldsim::behaviour::MoveThroughNetworkBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString geoworldsim::behaviour::MoveThroughNetworkBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";

geoworldsim::behaviour::MoveThroughNetworkBehaviour::MoveThroughNetworkBehaviour() : MoveBehaviour(){
}

geoworldsim::behaviour::MoveThroughNetworkBehaviour::~MoveThroughNetworkBehaviour(){
}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void geoworldsim::behaviour::MoveThroughNetworkBehaviour::afterCreateHook(){

    QSharedPointer<Entity> entity = this->getEntity();

    // Check if entity has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<skill::MoveThroughNetworkSkill> movethroughnetwork_skill = entity->getSkill( skill::MoveThroughNetworkSkill::staticMetaObject.className() , true ).dynamicCast<skill::MoveThroughNetworkSkill>();
    if( movethroughnetwork_skill.isNull() ){
        movethroughnetwork_skill = QSharedPointer<skill::MoveThroughNetworkSkill>( new skill::MoveThroughNetworkSkill() );
        entity->addSkill( movethroughnetwork_skill );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair<double, QJsonArray> geoworldsim::behaviour::MoveThroughNetworkBehaviour::behave(){

    QSharedPointer<Entity> entity = this->getEntity();
    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    unit::TimeUnit duration_of_movement = this->getProperty( BEHAVIOUR_DURATION ).toDouble( 1 );

    QSharedPointer<skill::MoveThroughNetworkSkill> movethroughroute_skill = entity->getSkill( skill::MoveThroughNetworkSkill::staticMetaObject.className() ).dynamicCast<skill::MoveThroughNetworkSkill>();

    geometry::Geometry route_destination = this->getProperty( INPUT_ROUTE_DESTINATION ).toObject();
    if( !route_destination.isValid() ){
        qWarning() << QString("Entity %1 %2 has invalid destination to route to").arg( entity->metaObject()->className() ).arg( entity->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    // Check Edge capacity
    QSharedPointer<Entity> current_edge = movethroughroute_skill->getCurrentEdge();
    int edge_capacity = -1;
    int entities_inside_count = -1;
    if( edge_capacity >= 0 && entities_inside_count >= edge_capacity ){
        // Wait until edge is free
        return QPair< double , QJsonArray >( 0 , QJsonArray({ this->getUID() }) );
    }

    // We can enter edge

    // Get all needed speeds
    unit::SpeedUnit current_speed = unit::SpeedUnit( entity->getProperty( skill::MoveSkill::INSTANT_SPEED ).toDouble( 0 ) );
    unit::SpeedUnit max_speed = unit::SpeedUnit( entity->getProperty( skill::MoveSkill::MAX_SPEED ).toDouble( 14 ) );
    if( current_edge ){ // TODO!
        max_speed = unit::SpeedUnit( current_edge->getProperty( skill::MoveSkill::MAX_SPEED ).toDouble( max_speed.number() ) );
    }

    // Accelerate or Brake
    if( current_speed == 0 ){
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , 1 );
    } else {
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , (max_speed.number() - current_speed.number()) / current_speed.number() );
    }
    entity->setProperty( skill::MoveSkill::INSTANT_SPEED , current_speed.number() );

    // Pending time to reach next route point can be higher than the duration requested.
    geometry::Geometry next_route_point = movethroughroute_skill->getCurrentMovingTowards();

    if( !next_route_point.isValid() ){
        next_route_point = route_destination;
    }

    unit::LengthUnit pending_distance = entity_geom.getDistance( next_route_point );
    unit::TimeUnit pending_time = pending_distance.number() / current_speed.number(); // Time needed to reach route_destination at current speed
    duration_of_movement = qMin( pending_time , duration_of_movement );

    // Get transport network graph type
    QString graph_type = this->getProperty( INPUT_TRANSPORT_NETWORK_TYPE ).toString();
    movethroughroute_skill->move( duration_of_movement , current_speed , route_destination , graph_type );

    // Once moved, check if arrived
    geometry::Geometry entity_geom_post = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Set NEXTS behaviour
    if ( entity_geom_post.getDistance( route_destination ) < unit::LengthUnit( 0.5 ) ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    return QPair< double , QJsonArray >( duration_of_movement.number() ,  this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
}
