#include "MoveSkill.h"

#include <QtMath>
#include "../../util/units/Units.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/geometry/GeometryTransformators.h"

QString geoworldsim::skill::MoveSkill::MAX_SPEED = "maxspeed";
QString geoworldsim::skill::MoveSkill::CURRENT_SPEED = "current_speed";
QString geoworldsim::skill::MoveSkill::MOVING_TOWARDS = "moving_towards";
QString geoworldsim::skill::MoveSkill::ACCUMULATED_DISTANCE = "accumulated_travelled_distance";
QString geoworldsim::skill::MoveSkill::ACCUMULATED_TIME = "accumulated_travelled_time";

geoworldsim::skill::MoveSkill::MoveSkill() : Skill(){
}

geoworldsim::skill::MoveSkill::~MoveSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

/*GWSSpeedUnit geoworldsim::skill::MoveSkill::getCurrentSpeed() {
    QSharedPointer<GWSAgent> agent = this->getAgent();
    return GWSSpeedUnit( agent->getProperty( STORE_CURRENT_SPEED_PROP ).toDouble() );
}

geometry::Coordinate geoworldsim::skill::MoveSkill::getMovingTowardsCoordinate(){
    QSharedPointer<GWSAgent> agent = this->getAgent();
    if( agent->getProperty( STORE_MOVING_TOWARDS_X_PROP ).isNull() || agent->getProperty( STORE_MOVING_TOWARDS_Y_PROP ).isNull() ){
        return geometry::Coordinate( NAN , NAN , NAN );
    }
    return geometry::Coordinate( agent->getProperty( STORE_MOVING_TOWARDS_X_PROP ) .toDouble( ) , agent->getProperty( STORE_MOVING_TOWARDS_Y_PROP ).toDouble( ) , 0 );
}*/

/**********************************************************************
 METHODS
**********************************************************************/

geoworldsim::unit::SpeedUnit geoworldsim::skill::MoveSkill::calculateNewSpeed(const geoworldsim::unit::SpeedUnit &current_speed, const geoworldsim::unit::SpeedUnit &max_speed, double force){
    double normalized_force = qMax( -1.0 , qMin( 1.0 , force ) );

    unit::SpeedUnit variation = max_speed * normalized_force;
    unit::SpeedUnit new_speed = current_speed + variation;
    new_speed = qMax( unit::SpeedUnit( 0 ) , new_speed );
    if( max_speed > unit::SpeedUnit( 0 ) && max_speed < new_speed ){ new_speed = max_speed; }

    return new_speed;
}

void geoworldsim::skill::MoveSkill::move(const geoworldsim::unit::TimeUnit &movement_duration, const geoworldsim::unit::SpeedUnit &movement_speed, const geoworldsim::geometry::Geometry &movement_towards_geom){

    QSharedPointer< Entity > entity = this->getEntity();
    entity->setProperties( QJsonObject({
         { CURRENT_SPEED , movement_speed.number() } ,
         { MOVING_TOWARDS , movement_towards_geom.getGeoJSON() }
                                      }) );

    if( movement_duration == 0 ){
        // Not move operation
        return;
    }

    double meters = movement_speed.number() // meters moved in 1 second
            * movement_duration.number();

    // Current position
    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    if( !entity_geom.isValid() ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( entity->metaObject()->className() ).arg( entity->getUID() );
    }

    // Get coors from geometries:
    const geometry::Coordinate& current_coor = entity_geom.getCentroid();
    const geometry::Coordinate& movement_towards_coor = movement_towards_geom.getCentroid();

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
        geometry::Coordinate apply_movement = geometry::Coordinate( x_move , y_move );
        geometry::Geometry moved_geometry = geometry::GeometryTransformators::transformMove( entity_geom , apply_movement );
        entity->setProperty( environment::PhysicalEnvironment::GEOMETRY_PROP , moved_geometry.getGeoJSON() );
    }

    entity->setProperty( ACCUMULATED_DISTANCE , entity->getProperty( ACCUMULATED_DISTANCE ).toDouble() + meters );
    entity->setProperty( ACCUMULATED_TIME , entity->getProperty( ACCUMULATED_TIME ).toDouble() + movement_duration.number() );

}
