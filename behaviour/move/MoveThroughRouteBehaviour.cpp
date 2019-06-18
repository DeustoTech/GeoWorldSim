#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString MoveThroughRouteBehaviour::INPUT_TRANSPORT_NETWORK_TYPE = "input_transport_network_type";
QString MoveThroughRouteBehaviour::INPUT_ROUTE_DESTINATION = "input_route_destination";
QString MoveThroughRouteBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";


MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : MoveBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void MoveThroughRouteBehaviour::afterCreateHook(){

    QSharedPointer<GWSEntity> entity = this->getEntity();

    // Check if entity has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = entity->getSkill( MoveThroughRouteSkill::staticMetaObject.className() , true ).dynamicCast<MoveThroughRouteSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteSkill>( new MoveThroughRouteSkill() );
        entity->addSkill( movethroughroute_skill );
    }

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > MoveThroughRouteBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();
    GWSGeometry entity_geom = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    GWSTimeUnit duration_of_movement = this->getProperty( BEHAVIOUR_DURATION ).toDouble( 1 );

    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = entity->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();

    GWSGeometry route_destination = this->getProperty( INPUT_ROUTE_DESTINATION ).toObject();
    if( !route_destination.isValid() ){
        qWarning() << QString("Entity %1 %2 has invalid destination to route to").arg( entity->metaObject()->className() ).arg( entity->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    // Check Edge capacity
    QSharedPointer<GWSEntity> current_edge = movethroughroute_skill->getCurrentEdge();
    int edge_capacity = -1;
    int entities_inside_count = -1;
    if( edge_capacity >= 0 && entities_inside_count >= edge_capacity ){
        // Wait until edge is free
        return QPair< double , QJsonArray >( 0 , QJsonArray({ this->getUID() }) );
    }

    // We can enter edge

    // Get all needed speeds
    GWSSpeedUnit current_speed = GWSSpeedUnit( entity->getProperty( MoveSkill::CURRENT_SPEED ).toDouble( 0 ) );
    GWSSpeedUnit max_speed = GWSSpeedUnit( entity->getProperty( MoveSkill::MAX_SPEED ).toDouble( 14 ) );
    if( current_edge ){ // TODO!
        max_speed = GWSSpeedUnit( current_edge->getProperty( MoveSkill::MAX_SPEED ).toDouble( max_speed.number() ) );
    }

    // Accelerate or Brake
    if( current_speed == 0 ){
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , 1 );
    } else {
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , (max_speed.number() - current_speed.number()) / current_speed.number() );
    }
    entity->setProperty( MoveSkill::CURRENT_SPEED , current_speed.number() );

    // Pending time to reach next route point can be higher than the duration requested.
    GWSGeometry next_route_point = movethroughroute_skill->getCurrentMovingTowards();

    if( !next_route_point.isValid() ){
        next_route_point = route_destination;
    }

    GWSLengthUnit pending_distance = entity_geom.getDistance( next_route_point );
    GWSTimeUnit pending_time = pending_distance.number() / current_speed.number(); // Time needed to reach route_destination at current speed
    duration_of_movement = qMin( pending_time , duration_of_movement );

    // Get transport network graph type
    QString graph_type = this->getProperty( INPUT_TRANSPORT_NETWORK_TYPE ).toString();
    movethroughroute_skill->move( duration_of_movement , current_speed , route_destination , graph_type );

    // Once moved, check if arrived
    GWSGeometry entity_geom_post = GWSGeometry( entity->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Set NEXTS behaviour
    if ( entity_geom_post.getDistance( route_destination ) < GWSLengthUnit( 0.5 ) ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    return QPair< double , QJsonArray >( duration_of_movement.number() ,  this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
}
