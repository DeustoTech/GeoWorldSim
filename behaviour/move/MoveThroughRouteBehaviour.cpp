#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString MoveThroughRouteBehaviour::TRANSPORT_NETWORK_TYPE = "transport_network_type";
QString MoveThroughRouteBehaviour::AGENT_ROUTE_DESTINATION_X_VALUE = "route_destination_x_value";
QString MoveThroughRouteBehaviour::AGENT_ROUTE_DESTINATION_Y_VALUE = "route_destination_y_value";
QString MoveThroughRouteBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";


MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : MoveBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void MoveThroughRouteBehaviour::initialize(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteSkill>( new MoveThroughRouteSkill() );
        agent->addSkill( movethroughroute_skill );
    }

    // Check if agent has NoiseSkill, otherwise create it and set the agent type

    // Check if agent has PolluteSkill

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > MoveThroughRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = this->getProperty( BEHAVIOUR_DURATION ).toDouble( 1 );  //qrand() % 100 / 100.0;

    QJsonValue x_destination = this->getProperty( AGENT_ROUTE_DESTINATION_X_VALUE );
    QJsonValue y_destination = this->getProperty( AGENT_ROUTE_DESTINATION_Y_VALUE );
    GWSCoordinate destination_coor = GWSCoordinate( x_destination.toDouble() , y_destination.toDouble() );
    if( !destination_coor.isValid() ){
        qWarning() << QString("Agent %1 %2 has invalid destination to route to").arg( agent->metaObject()->className() ).arg( agent->getUID() );
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

    // Calculate speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    GWSSpeedUnit current_speed = GWSSpeedUnit( this->getProperty( AGENT_CURRENT_SPEED ).toDouble() );
    GWSSpeedUnit max_speed = GWSSpeedUnit( this->getProperty( AGENT_MAX_SPEED ).toDouble() );

    if( current_speed == 0 ){
        current_speed = movethroughroute_skill->calculateNewSpeed( current_speed , max_speed , 1.4 );
        this->setProperty( AGENT_CURRENT_SPEED , current_speed.number() );
    }

    // Pending time to reach destination can be higher than the duration requested.
    GWSCoordinate agent_position = agent_geom.getCentroid();
    GWSLengthUnit pending_distance = agent_position.getDistance( destination_coor );
    GWSTimeUnit pending_time = pending_distance.number() / current_speed.number(); // Time needed to reach route_destination at current speed
    duration_of_movement = qMin( pending_time , duration_of_movement );

    // Move towards
    QString graph_type = this->getProperty( TRANSPORT_NETWORK_TYPE ).toString();

    movethroughroute_skill->move( duration_of_movement , current_speed , destination_coor , graph_type );

    GWSGeometry agent_geom_post = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );

    GWSCoordinate agent_position_post = agent_geom_post.getCentroid();

    // Set NEXTS behaviour
    if ( agent_position_post.getDistance( destination_coor ) < GWSLengthUnit( 0.5 ) ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_ARRIVED ).toArray() );
    }

    if ( agent_position_post != destination_coor ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() ,  this->getProperty( NEXTS_IF_NOT_ARRIVED ).toArray() );
    }

}
