#include "DriveBehaviour.h"
#include "../../skill/move/drive/DriveSkill.h"

QString DriveBehaviour::VEHICLE_TYPE = "vehicle_type";
QString DriveBehaviour::VEHICLE_SUBTYPE = "vehicle_subtype";
QString DriveBehaviour::VEHICLE_LOAD = "vehicle_load";
QString DriveBehaviour::CURRENT_SPEED = "current_speed";
QString DriveBehaviour::CURRENT_ROAD = "current_road";
QString DriveBehaviour::CURRENT_ROAD_MAXSPEED = "current_road_maxspeed";
QString DriveBehaviour::SPEED_FACTOR_PROP = "speed_factor";
QString DriveBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString DriveBehaviour::NEXTS_IF_FALSE = "nexts_if_false";


DriveBehaviour::DriveBehaviour() : MoveThroughRouteBehaviour(){

}


QStringList DriveBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // ---------------------------------------
    // Get road type to extract maximum speed
    // ---------------------------------------
    QVariant current_road = this->getProperty( CURRENT_ROAD );

    bool road_is_property = current_road.toString().startsWith( "<" ) && current_road.toString().endsWith( ">" );

    if ( road_is_property ){
        QString property_name = current_road.toString().remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        current_road = agent->getProperty( property_name );
    }

    QString road = current_road.toString();
    double road_speed = agent->getProperty( "road_speed" ).toDouble();

    // ---------------------------------------
    // Get vehicle speed
    // ---------------------------------------
    QVariant current_speed = this->getProperty( CURRENT_SPEED );

    bool speed_is_property = current_speed.toString().startsWith( "<" ) && current_speed.toString().endsWith( ">" );

    if ( speed_is_property ){
        QString property_name = current_speed.toString().remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        current_speed = agent->getProperty( property_name );
    }

    GWSSpeedUnit speed = current_speed.toDouble();


    // ---------------------------------------
    // Get road maxspeed
    // ---------------------------------------
    QVariant current_road_maxspeed = this->getProperty( CURRENT_SPEED );


    // ---------------------------------------------------------------------------
    // Check if agent has a DriveSkill, otherwise create it and set its max_speed
    // ---------------------------------------------------------------------------

    QSharedPointer<DriveSkill> drive_skill = agent->getSkill( DriveSkill::staticMetaObject.className() ).dynamicCast<DriveSkill>();
    if( drive_skill.isNull() ){
        drive_skill = QSharedPointer<DriveSkill>( new DriveSkill() );
        agent->addSkill( drive_skill );
    }
    drive_skill->setProperty( DriveSkill::ADJUST_TO_SPEED_LIMITS_PROP , this->getProperty( DriveBehaviour::SPEED_FACTOR_PROP ) );

    double acceleration = drive_skill->calculateAccelerateForce( speed, road_speed , 50);
    qDebug() << acceleration;


    // Get all vehicles in our road

    // Calculate nearest vehicle

    // Set current speed


    return MoveThroughRouteBehaviour::behave();
}
