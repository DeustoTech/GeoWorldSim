#include "MoveSkill.h"

#include <QtMath>

#include "../../environment/Environment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

QString MoveSkill::MAX_SPEED_PROP = "maxspeed";
QString MoveSkill::CURRENT_SPEED_PROP = "speed";
QString MoveSkill::ACCUMULATED_DISTANCE_PROP = "accumulated_distance";
QString MoveSkill::ACCUMULATED_TIME_PROP = "accumulated_time";
QString MoveSkill::DESTINATION_X_PROP = "destination_x";
QString MoveSkill::DESTINATION_Y_PROP = "destination_y";


MoveSkill::MoveSkill() : GWSSkill(){
    this->setProperty( MoveSkill::MAX_SPEED_PROP , GWSSpeedUnit( 4 / 3.6 ) );
    this->setProperty( MoveSkill::CURRENT_SPEED_PROP , GWSSpeedUnit( 0 ) );
    this->setProperty( MoveSkill::ACCUMULATED_DISTANCE_PROP , GWSLengthUnit(0) );
    this->setProperty( MoveSkill::ACCUMULATED_TIME_PROP , GWSTimeUnit(0) );
}

MoveSkill::~MoveSkill(){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSSpeedUnit MoveSkill::getMaxSpeed() const {
    return this->getProperty( MoveSkill::MAX_SPEED_PROP ).toDouble();
}

GWSSpeedUnit MoveSkill::getCurrentSpeed() const{
    return this->getProperty( MoveSkill::CURRENT_SPEED_PROP ).toDouble();
}

GWSLengthUnit MoveSkill::getAccDistance() const{
    return this->getProperty( MoveSkill::ACCUMULATED_DISTANCE_PROP ).toDouble();
}

GWSTimeUnit MoveSkill::getAccTime() const{
    return this->getProperty( MoveSkill::ACCUMULATED_TIME_PROP ).toDouble();
}

GWSCoordinate MoveSkill::getDestination() const{
    if( this->getProperty( DESTINATION_X_PROP ).isNull() || this->getProperty( DESTINATION_Y_PROP ).isNull() ){
        return GWSCoordinate( NAN , NAN , NAN );
    }
    return GWSCoordinate( this->getProperty( DESTINATION_X_PROP ) .toDouble( ) , this->getProperty( DESTINATION_Y_PROP ).toDouble( ) , 0 );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void MoveSkill::setMaxSpeed(GWSSpeedUnit max_speed){
    this->setProperty( MoveSkill::MAX_SPEED_PROP , max_speed );
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSSpeedUnit MoveSkill::changeSpeed(double force){
    double normalized_force = qMax( -1.0 , qMin( 1.0 , force ) );

    GWSSpeedUnit current_speed = this->getCurrentSpeed();
    GWSSpeedUnit max_speed = this->getMaxSpeed();
    GWSSpeedUnit variation = max_speed * normalized_force;
    GWSSpeedUnit new_speed = current_speed + variation;
    new_speed = qMax( GWSSpeedUnit( 0 ) , new_speed );
    new_speed = qMin( max_speed , new_speed );

    this->setProperty( MoveSkill::CURRENT_SPEED_PROP , new_speed );
    //this->skilled_agent->getStyle()->color = this->skilled_agent->getStyle()->border_color = QColor( 255 * (  ) , 255 , 128 );

    emit this->speedChangedSignal( new_speed );
    return new_speed;
}

GWSSpeedUnit MoveSkill::brakeToStop(){
    this->setProperty( MoveSkill::CURRENT_SPEED_PROP , GWSSpeedUnit( 0 ) );
    emit this->speedChangedSignal( this->getCurrentSpeed() );
    return GWSSpeedUnit( 0 );
}

void MoveSkill::move( GWSTimeUnit movement_duration ){

    GWSSpeedUnit speed = this->getCurrentSpeed();
    double meters = speed.number() // meters moved in 1 second
            * movement_duration.number();

    // Current position
    QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSGeometry> agent_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    if( !agent_geom ){
        qWarning() << QString("Agent %1 %2 tried to move without geometry").arg( agent->metaObject()->className() ).arg( agent->getId() );
    }
    GWSCoordinate current_coor = agent_geom->getCentroid();
    GWSCoordinate destination_coor = this->getDestination();

    // Distance
    double meter_distance = current_coor.getDistance( this->getDestination() ).number();
    double distance_percentage = (meters / meter_distance);

    distance_percentage = qMin( distance_percentage , 1.0 );

    // Displacement
    double x_distance = qAbs( destination_coor.getX() - current_coor.getX() );
    double y_distance = qAbs( destination_coor.getY() - current_coor.getY() );
    double x_move = x_distance * distance_percentage * ( destination_coor.getX() > current_coor.getX() ? 1 : -1 );
    double y_move = y_distance * distance_percentage * ( destination_coor.getY() > current_coor.getY() ? 1 : -1 );

    // Set the agents position
    GWSCoordinate position = GWSCoordinate( x_move , y_move );
    agent_geom->transformMove( position );
    qDebug() << "Step = " <<position.toString();
    this->setProperty( ACCUMULATED_DISTANCE_PROP , this->getAccDistance() + meters );
    this->setProperty( ACCUMULATED_TIME_PROP , this->getAccTime() + movement_duration );

    // Lose some speed
    //this->setProperty( CURRENT_SPEED_PROP , this->getCurrentSpeed() * 0.9 );
}
