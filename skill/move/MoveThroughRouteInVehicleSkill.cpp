#include "MoveThroughRouteInVehicleSkill.h"

MoveThroughRouteInVehicleSkill::MoveThroughRouteInVehicleSkill() : MoveThroughRouteSkill ()
{

}


void MoveThroughRouteInVehicleSkill::moveAndPollute( GWSTimeUnit movement_duration ){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    MoveThroughRouteSkill::move( movement_duration );

    QSharedPointer<PolluteSkill> vehiclePollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();
    if( vehiclePollute_skill.isNull() ){
        vehiclePollute_skill = QSharedPointer<PolluteSkill>( new PolluteSkill() );
        agent->addSkill( vehiclePollute_skill );
    }

    QString vehicle_subtype = agent->getProperty( "vehicleSubtype" ).toString();


    // Pollute skill for each pollutant of choice:
    vehiclePollute_skill->pollute( vehicle_subtype , "CO" ,   agent->getProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP ).isDouble() , 2.0 , agent->getProperty( AGENT_CURRENT_ROAD_TYPE_PROP ).toString() , 0.66 );
    vehiclePollute_skill->pollute( vehicle_subtype , "HC" ,   agent->getProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP ).isDouble(), 2.0 , agent->getProperty( AGENT_CURRENT_ROAD_TYPE_PROP ).toString() , 0.66 );



}
