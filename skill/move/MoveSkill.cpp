#include "MoveSkill.h"

#include <QtMath>

#include "../../environment/Environment.h"
#include "../../app/App.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryTransformators.h"

QString MoveSkill::MAX_SPEED = "maxspeed";
QString MoveSkill::CURRENT_SPEED = "current_speed";
QString MoveSkill::MOVING_TOWARDS = "moving_towards";
QString MoveSkill::ACCUMULATED_DISTANCE = "accumulated_travelled_distance";
QString MoveSkill::ACCUMULATED_TIME = "accumulated_travelled_time";

MoveSkill::MoveSkill() : GWSSkill(){
}

MoveSkill::~MoveSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/*GWSSpeedUnit MoveSkill::getCurrentSpeed() {
    QSharedPointer<GWSAgent> agent = this->getAgent();
    return GWSSpeedUnit( agent->getProperty( STORE_CURRENT_SPEED_PROP ).toDouble() );
}

GWSCoordinate MoveSkill::getMovingTowardsCoordinate(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    if( agent->getProperty( STORE_MOVING_TOWARDS_X_PROP ).isNull() || agent->getProperty( STORE_MOVING_TOWARDS_Y_PROP ).isNull() ){
        return GWSCoordinate( NAN , NAN , NAN );
    }
    return GWSCoordinate( agent->getProperty( STORE_MOVING_TOWARDS_X_PROP ) .toDouble( ) , agent->getProperty( STORE_MOVING_TOWARDS_Y_PROP ).toDouble( ) , 0 );
}*/

/**********************************************************************
 METHODS
**********************************************************************/

GWSSpeedUnit MoveSkill::calculateNewSpeed(const GWSSpeedUnit &current_speed, const GWSSpeedUnit &max_speed, double force){
    double normalized_force = qMax( -1.0 , qMin( 1.0 , force ) );

    GWSSpeedUnit variation = max_speed * normalized_force;
    GWSSpeedUnit new_speed = current_speed + variation;
    new_speed = qMax( GWSSpeedUnit( 0 ) , new_speed );
    if( max_speed < new_speed ){ new_speed = max_speed; }

    emit this->speedChangedSignal( new_speed );
    return new_speed;
}

void MoveSkill::move(const GWSTimeUnit &movement_duration, const GWSSpeedUnit &movement_speed, const GWSGeometry &movement_towards_geom){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    agent->setProperty( CURRENT_SPEED , movement_speed );
    agent->setProperty( MOVING_TOWARDS , movement_towards_geom.getGeoJSON() );

    if( movement_duration == 0 ){
        // Not move operation
        return;
    }

    double meters = movement_speed.number() // meters moved in 1 second
            * movement_duration.number();

    // Current position
    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    if( !agent_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getUID() );
    }

    // Get coors from geometries:
    const GWSCoordinate& current_coor = agent_geom.getCentroid();
    const GWSCoordinate& movement_towards_coor = movement_towards_geom.getCentroid();

    // Distance
    double meter_distance = current_coor.getDistance( movement_towards_coor ).number();

    // Check if not arrived
    if( meter_distance > 0 ){

        double distance_percentage = ( meters / meter_distance );

        // Check if we are overpassing the movement_towards coordinate.
        // If so, set to destination
        if( distance_percentage > 1 ){
            distance_percentage = 1;
        }

        // Displacement
        double x_distance = qAbs( movement_towards_coor.getX() - current_coor.getX() );
        double y_distance = qAbs( movement_towards_coor.getY() - current_coor.getY() );

        double x_move = x_distance * distance_percentage * ( movement_towards_coor.getX() > current_coor.getX() ? 1 : -1 );
        double y_move = y_distance * distance_percentage * ( movement_towards_coor.getY() > current_coor.getY() ? 1 : -1 );

        // Set the agents position
        GWSCoordinate apply_movement = GWSCoordinate( x_move , y_move );
        GWSGeometry moved_geometry = GWSGeometryTransformators::transformMove( agent_geom , apply_movement );
        agent->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , moved_geometry.getGeoJSON() );
    }

    agent->setProperty( ACCUMULATED_DISTANCE , agent->getProperty( ACCUMULATED_DISTANCE ).toDouble() + meters );
    agent->setProperty( ACCUMULATED_TIME , agent->getProperty( ACCUMULATED_TIME ).toDouble() + movement_duration.number() );

}
