#include "MoveSkill.h"

#include <QtMath>

#include "../../environment/Environment.h"
#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString MoveSkill::AGENT_MAX_SPEED_PROP = "maxspeed";
QString MoveSkill::AGENT_CURRENT_SPEED_PROP = "current_speed";
QString MoveSkill::AGENT_MOVING_TOWARDS_X_PROP = "agent_moving_towards_x";
QString MoveSkill::AGENT_MOVING_TOWARDS_Y_PROP = "agent_moving_towards_y";
QString MoveSkill::AGENT_ACCUMULATED_DISTANCE_PROP = "total_moved_distance";
QString MoveSkill::AGENT_ACCUMULATED_TIME_PROP = "total_travel_time";

MoveSkill::MoveSkill() : GWSSkill(){
}

MoveSkill::~MoveSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSSpeedUnit MoveSkill::getCurrentSpeed() {
    QSharedPointer<GWSAgent> agent = this->getAgent();
    return GWSSpeedUnit( agent->getProperty( AGENT_CURRENT_SPEED_PROP ).toDouble() );
}

GWSCoordinate MoveSkill::getMovingTowardsCoordinate(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    if( agent->getProperty( AGENT_MOVING_TOWARDS_X_PROP ).isNull() || agent->getProperty( AGENT_MOVING_TOWARDS_Y_PROP ).isNull() ){
        return GWSCoordinate( NAN , NAN , NAN );
    }
    return GWSCoordinate( agent->getProperty( AGENT_MOVING_TOWARDS_X_PROP ) .toDouble( ) , agent->getProperty( AGENT_MOVING_TOWARDS_Y_PROP ).toDouble( ) , 0 );
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSSpeedUnit MoveSkill::changeSpeed(double force){
    double normalized_force = qMax( -1.0 , qMin( 1.0 , force ) );

    QSharedPointer<GWSAgent> agent = this->getAgent();

    GWSSpeedUnit current_speed = agent->getProperty( AGENT_CURRENT_SPEED_PROP ).toDouble();
    GWSSpeedUnit max_speed = agent->getProperty( AGENT_MAX_SPEED_PROP ).toDouble();
    GWSSpeedUnit variation = max_speed * normalized_force;
    GWSSpeedUnit new_speed = current_speed + variation;
    new_speed = qMax( GWSSpeedUnit( 0 ) , new_speed );
    new_speed = qMin( max_speed , new_speed );


    if ( force < 0 ){
        agent->setProperty( "color" , "Red" );
    }
    else {
        agent->setProperty( "color" , "Blue" );
    }
    //this->skilled_agent->getStyle()->color = this->skilled_agent->getStyle()->border_color = QColor( 255 * (  ) , 255 , 128 );

    emit this->speedChangedSignal( new_speed );
    return new_speed;
}


void MoveSkill::move( GWSTimeUnit movement_duration , GWSSpeedUnit movement_speed , GWSCoordinate movement_towards ){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty( AGENT_CURRENT_SPEED_PROP , movement_speed );
    agent->setProperty( AGENT_MOVING_TOWARDS_X_PROP , movement_towards.getX() );
    agent->setProperty( AGENT_MOVING_TOWARDS_Y_PROP , movement_towards.getY() );

    if( movement_duration == 0 ){
        // Not move operation
        return;
    }

    double meters = movement_speed.number() // meters moved in 1 second
            * movement_duration.number();

    // Current position
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
    }
    GWSCoordinate current_coor = agent_geom->getCentroid();

    // Distance
    double meter_distance = current_coor.getDistance( movement_towards ).number();

    // Check if not arrived
    if( meter_distance > 0 ){

        double distance_percentage = ( meters / meter_distance );

        // Check if we are overpassing the movement_towards coordinate.
        // If so, set to destination
        if( distance_percentage > 1 ){
            distance_percentage = 1;
        }

        // Displacement
        double x_distance = qAbs( movement_towards.getX() - current_coor.getX() );
        double y_distance = qAbs( movement_towards.getY() - current_coor.getY() );

        double x_move = x_distance * distance_percentage * ( movement_towards.getX() > current_coor.getX() ? 1 : -1 );
        double y_move = y_distance * distance_percentage * ( movement_towards.getY() > current_coor.getY() ? 1 : -1 );

        // Set the agents position
        GWSCoordinate apply_movement = GWSCoordinate( x_move , y_move );
        GWSPhysicalEnvironment::globalInstance()->transformMove( agent , apply_movement );

    }

    agent->setProperty( AGENT_ACCUMULATED_DISTANCE_PROP , agent->getProperty( AGENT_ACCUMULATED_DISTANCE_PROP ).toDouble() + meters );
    agent->setProperty( AGENT_ACCUMULATED_TIME_PROP , agent->getProperty( AGENT_ACCUMULATED_TIME_PROP ).toDouble() + movement_duration.number() );

}
