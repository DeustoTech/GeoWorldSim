#include "MoveBehaviour.h"

#include "../../environment/time_environment/TimeEnvironment.h"


QString MoveBehaviour::DESTINATION_X = "destination_x";
QString MoveBehaviour::DESTINATION_Y = "destination_y";

MoveBehaviour::MoveBehaviour(){

}

/**********************************************************************
 GETTERS
**********************************************************************/

bool MoveBehaviour::finished(){
    if( this->getProperty( DESTINATION_X ).isNull() || this->getProperty( DESTINATION_Y ).isNull() ){
        return true;
    }
    return this->getAgent()->getGeometry()->getCentroid() == GWSCoordinate( this->getProperty( DESTINATION_X ).toDouble() , this->getProperty( DESTINATION_Y ) );
}

/**********************************************************************
 METHODS
**********************************************************************/


bool MoveBehaviour::behave(){

    // Calculate environment tick time
    double tick_duration = 1000 / GWSTimeEnvironment::globalInstance()->getTimeSpeed();

    // Calculate how much the agent can move in that time

    // Set the agents position

    // Increment internal time
    this->getAgent()->incrementInternalTime( tick_duration );
}
