#include "MoveThroughRouteBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/move/MoveThroughRouteSkill.h"


QString MoveThroughRouteBehaviour::MAX_SPEED_PROP = "maxspeed";
QString MoveThroughRouteBehaviour::X_VALUE = "x_value";
QString MoveThroughRouteBehaviour::Y_VALUE = "y_value";
QString MoveThroughRouteBehaviour::NEXTS_IF_ARRIVED = "nexts_if_arrived";
QString MoveThroughRouteBehaviour::NEXTS_IF_NOT_ARRIVED = "nexts_if_not_arrived";
QString MoveThroughRouteBehaviour::STORE_MOVED_DISTANCE_AS = "store_total_moved_distance_as";
QString MoveThroughRouteBehaviour::STORE_TRAVEL_TIME_AS = "store_total_travel_time_as";

MoveThroughRouteBehaviour::MoveThroughRouteBehaviour() : GWSBehaviour(){
}

MoveThroughRouteBehaviour::~MoveThroughRouteBehaviour(){
}


/**********************************************************************
 METHODS
**********************************************************************/

QStringList MoveThroughRouteBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Tick in 1 second duration to move in small parts
    GWSTimeUnit duration_of_movement = qrand() % 100 / 100.0;

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    if( movethroughroute_skill.isNull() ){
        movethroughroute_skill = QSharedPointer<MoveThroughRouteSkill>( new MoveThroughRouteSkill() );
        agent->addSkill( movethroughroute_skill );
    }
    movethroughroute_skill->setProperty( MoveThroughRouteSkill::MAX_SPEED_PROP , this->getProperty( MAX_SPEED_PROP ) );

    QVariant x_destination = this->getProperty( X_VALUE );
    QVariant y_destination = this->getProperty( Y_VALUE );

    bool x_is_property = x_destination.toString().startsWith( "<" ) && x_destination.toString().endsWith( ">" );
    bool y_is_property = y_destination.toString().startsWith( "<" ) && y_destination.toString().endsWith( ">" );

    if ( x_is_property && y_is_property ){

        QString x_property_name = x_destination.toString().remove( 0 , 1 );
        QString y_property_name = y_destination.toString().remove( 0 , 1 );

        x_property_name = x_property_name.remove( x_property_name.length() - 1 , 1 );
        y_property_name = y_property_name.remove( y_property_name.length() - 1 , 1 );

        x_destination = agent->getProperty( x_property_name );
        y_destination = agent->getProperty( y_property_name );

    }

    movethroughroute_skill->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_X_PROP , x_destination );
    movethroughroute_skill->setProperty( MoveThroughRouteSkill::ROUTE_DESTINATION_Y_PROP , y_destination );

    QStringList nexts;
    GWSCoordinate destination_coor = movethroughroute_skill->getRouteDestination();
    if( !destination_coor.isValid() ){
        nexts = this->getProperty( NEXTS_IF_NOT_ARRIVED ).toStringList();
    }

    // Calculate speed
    GWSLengthUnit distance = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getCentroid().getDistance( destination_coor );
    if( movethroughroute_skill->getCurrentSpeed() == 0.0 ){
        movethroughroute_skill->changeSpeed( 1 );
    }

    QSharedPointer<GWSGeometry> agent_geom_init = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_init = agent_geom_init->getCentroid();


    // Move towards
    movethroughroute_skill->move( duration_of_movement );

    QSharedPointer<GWSGeometry> agent_geom_post = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    GWSCoordinate agent_position_post = agent_geom_post->getCentroid();

    // Calculate moved distance and store in accumulated distance:
    GWSLengthUnit moved_distance;
    moved_distance = agent_position_post.getDistance( agent_position_init );

    GWSLengthUnit accumulated_distance;
    accumulated_distance = moved_distance + agent->getProperty(this->getProperty( STORE_MOVED_DISTANCE_AS ).toString() ).toDouble();
    agent->setProperty( this->getProperty( STORE_MOVED_DISTANCE_AS ).toString() , accumulated_distance );

    // Calculate travel time and store in accumulated time:
    GWSTimeUnit travel_time;
    GWSSpeedUnit speed = movethroughroute_skill->getCurrentSpeed();
    travel_time = moved_distance / speed.number();

    GWSTimeUnit accumulated_travel_time;
    accumulated_travel_time = travel_time + agent->getProperty(this->getProperty( STORE_TRAVEL_TIME_AS ).toString() ).toDouble();
    agent->setProperty( this->getProperty( STORE_TRAVEL_TIME_AS ).toString() , accumulated_travel_time );


    // Set NEXTS behaviour
    if ( agent_position_post == destination_coor ){
        nexts = this->getProperty( NEXTS_IF_ARRIVED ).toStringList();
    }

    if ( agent_position_post != destination_coor ){
        nexts = this->getProperty( NEXTS_IF_NOT_ARRIVED ).toStringList();
    }

   emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent ->serialize() );
   return nexts;
}
