#include "MoveThroughRouteInVehicleSkill.h"

MoveThroughRouteInVehicleSkill::MoveThroughRouteInVehicleSkill() : MoveThroughRouteSkill(){

}


void MoveThroughRouteInVehicleSkill::move(GWSTimeUnit movement_duration, GWSSpeedUnit movement_speed, GWSCoordinate route_destination, QString graph_type) {

    QSharedPointer<GWSAgent> agent = this->getAgent();

    MoveThroughRouteSkill::move( movement_duration , movement_speed , route_destination , graph_type );

    /*QSharedPointer<PolluteSkill> vehiclePollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();
    if( vehiclePollute_skill.isNull() ){
        vehiclePollute_skill = QSharedPointer<PolluteSkill>( new PolluteSkill() );
        qDebug() << "Adding PolluteSkill";
        agent->addSkill( vehiclePollute_skill );
    }

    QString vehicle_subtype = agent->getProperty( "vehicleSubtype" ).toString();
    QString road_type = agent->getProperty( STORE_CURRENT_ROAD_TYPE ).toString();

    // Pollute skill for each pollutant of choice:
    qDebug() << vehicle_subtype << road_type;
    if ( !road_type.isEmpty() ){
        vehiclePollute_skill->pollute( vehicle_subtype , "CO" ,   agent->getProperty( MoveSkill::STORE_CURRENT_SPEED_PROP ).isDouble() , 2.0 , road_type , 0.66 );
        vehiclePollute_skill->pollute( vehicle_subtype , "HC" ,   agent->getProperty( MoveSkill::STORE_CURRENT_SPEED_PROP ).isDouble() , 2.0 , road_type , 0.66 );
    }
*/



}
