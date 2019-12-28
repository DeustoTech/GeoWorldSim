#include "MoveThroughTransportLineSkill.h"

#include "environment/physical_environment/PhysicalEnvironment.h"

#include "agents/vehicles/Vehicle.h"
#include "agents/people/vehicle_drivers/VehicleDriver.h"

#include "skills/store/UnitStoreSkill.h"
#include "skills/produce/pollution/vehicle/VehiclePollutionProduceSkill.h"

MoveThroughTransportLineSkill::MoveThroughTransportLineSkill(Vehicle *skilled_agent) : MoveSkill( skilled_agent ){
}

MoveThroughTransportLineSkill::~MoveThroughTransportLineSkill(){
    //if( this->current_edge ){ this->current_edge->exit( this->skilled_agent ); }
}

/**********************************************************************
 METHODS
**********************************************************************/

void MoveThroughTransportLineSkill::moveTo(TransportLine *transport_line){

    if( this->skilled_agent->getGeometry()->equals( transport_line->getGeometry() ) ){
        return;
    }
    Vehicle* vehicle = dynamic_cast<Vehicle*>( this->skilled_agent );

    // Enter new edge
    this->current_edge = transport_line;

    GWSSpeedUnit current_speed = this->getCurrentSpeed();
    double current_traffic = vehicle->property( DriveSkill::SPEED_FACTOR_PROP ).toDouble();

    double meters = 0 , seconds = 0;

    if( current_speed > GWSSpeedUnit( 0 ) ){
        // Calculate how much will it take according to CURRENT_SPEED_PROP
        meters = transport_line->getLength().number();
        seconds =  meters / current_speed.number();
    }

    // Set new position
    GWSPhysicalEnvironment::globalInstance()->updateAgentGeometry( this->skilled_agent , transport_line->getGeometry()->createCopy() );

    // Set inside elements position
    foreach(GWSSkill* s , this->skilled_agent->getSkills( AgentStoreSkill::staticMetaObject.className() )){
        foreach(GWSAgent* a , dynamic_cast<AgentStoreSkill*>( s )->getAgentsInside() ){
            GWSPhysicalEnvironment::globalInstance()->updateAgentGeometry( a , this->skilled_agent->getGeometry()->createCopy() );
        }
    }

    this->setProperty( MoveSkill::ACCUMULATED_DISTANCE_PROP , GWSLengthUnit( meters + this->getAccDistance().number() ) );
    this->setProperty( MoveSkill::ACCUMULATED_TIME_PROP , GWSTimeUnit( seconds + this->getAccTime().number() ) );

    // See if it pollutes
    double load = 0.5;
    foreach( GWSSkill* s , this->skilled_agent->getSkills( VehiclePollutionProduceSkill::staticMetaObject.className() ) ){
        VehiclePollutionProduceSkill* ps = dynamic_cast<VehiclePollutionProduceSkill*>( s );
        ps->produce( transport_line , current_speed , current_traffic , transport_line->getGradient() , load , transport_line->getLength() );
    }

    // Do not call until reached new position
    vehicle->incrementNextTick( seconds );

    // Increment for driver
    vehicle->getDriver()->incrementNextTick( seconds );

    emit this->movedSignal();
}
