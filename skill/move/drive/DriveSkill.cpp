#include "DriveSkill.h"

QString DriveSkill::SPEED_FACTOR_PROP = "speed_factor";
QString DriveSkill::STOP_DISTANCE_PROP = "stop_distance";

DriveSkill::DriveSkill(GWSAgent *parent) : GWSSkill(parent){
    // Driver
    this->setProperty( DriveSkill::SPEED_FACTOR_PROP , 1 );  // It will go to the 100% of the speed
    this->setProperty( DriveSkill::STOP_DISTANCE_PROP , GWSLengthUnit( 50 ) ); // Likes to start stopping 50 meters ahead
}

/**********************************************************************
 GETTERS
**********************************************************************/

double DriveSkill::getSpeedFactor() const{
    return this->property( DriveSkill::SPEED_FACTOR_PROP ).toDouble();
}

GWSLengthUnit DriveSkill::getStopDistanceFactor() const{
    return this->property( DriveSkill::STOP_DISTANCE_PROP ).value<GWSUnit>();
}

/**********************************************************************
 SETTERS
**********************************************************************/

void DriveSkill::setSpeedFactor(double factor){
    this->setProperty( DriveSkill::SPEED_FACTOR_PROP , factor );
}

void DriveSkill::setStopDistance(GWSLengthUnit distance){
    this->setProperty( DriveSkill::STOP_DISTANCE_PROP , distance );
}

/**********************************************************************
 METHODS
**********************************************************************/

double DriveSkill::calculateAccelerateForce(GWSSpeedUnit vehicle_speed, GWSSpeedUnit road_max_speed , GWSLengthUnit free_distance){

    GWSSpeedUnit should_go_speed = road_max_speed * this->getSpeedFactor();

    if( should_go_speed == GWSSpeedUnit( 0 ) ){
        qWarning() << "Should go speed is 0";
    }
    if( free_distance <= GWSLengthUnit( 0 )){
        return -1;
    }

    // Accellerate
    if( vehicle_speed < should_go_speed ){
        double acceleration_force = (should_go_speed - vehicle_speed).number() / should_go_speed.number();
        return qMin( acceleration_force , 1.0 );
    }

    // Brake
    if( vehicle_speed > should_go_speed ){
        double brake_force = -1 * (vehicle_speed - should_go_speed).number() / vehicle_speed.number();
        return qMax( brake_force , -1.0 );
    }

    return 0;


    // TODO FIX
    GWSSpeedUnit adjusted_road_max_speed = road_max_speed * this->getSpeedFactor();
    return qMax( -1.0 , (adjusted_road_max_speed - vehicle_speed).number() / adjusted_road_max_speed.number() );
}
