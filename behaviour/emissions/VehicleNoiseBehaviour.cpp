#include "VehicleNoiseBehaviour.h"

#include <QtMath>

#include "../../skill/move/MoveSkill.h"
#include "../../object/ObjectFactory.h"

QString VehicleNoiseBehaviour::INPUT_VEHICLE_TYPE = "input_transport_mode";
QString VehicleNoiseBehaviour::OUTPUT_NOISE = "output_noise";
QString VehicleNoiseBehaviour::NEXTS = "nexts";

VehicleNoiseBehaviour::VehicleNoiseBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > VehicleNoiseBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double A_roll = 0;
    double B_roll = 0;
    double A_prop = 0;
    double B_prop = 0;
    double vref = 70.0;

    GWSSpeedUnit vehicle_speed = agent->getProperty( MoveSkill::CURRENT_SPEED ).toDouble();
    QString vehicle_type = this->getProperty( INPUT_VEHICLE_TYPE ).toString();

    double total_noise;

    if( vehicle_type.toUpper() == "CAR" ){
        A_roll = 93.9;
        B_roll = 37.2;
        A_prop = 86.9;
        B_prop = 8.0;
    }

    if( vehicle_type.toUpper() == "ECAR" ){
        A_roll = 93.9;
        B_roll = 37.2;
        A_prop = 0;
        B_prop = 0;
    }


    if( vehicle_type.toUpper() == "MOTORCYCLE" ){
        A_roll = 0;
        B_roll = 0;
        A_prop = 93.2;
        B_prop = 12.6;
    }

    // Rolling Noise:
    double rolling_noise = A_roll + B_roll * log10( vehicle_speed.number() / vref );

    // Propulsion Noise:
    double propulsion_noise = A_prop + B_prop * ( vehicle_speed.number() - vref ) / vref;

    // Total Noise:
    if ( vehicle_type == "WALK" || vehicle_type == "BICYCLE" ){
        total_noise = 0;
    }
    else{
        total_noise = 10 * log10( qPow( 10 , rolling_noise / 10.0 )  +  qPow( 10 , propulsion_noise / 10.0 ) );
    }

    // Store noise
    double existing_value = agent->getProperty( "vehicle_noise" ).toDouble();
    agent->setProperty( this->getProperty( OUTPUT_NOISE ).toString( "vehicle_noise" ) , total_noise );
    agent->setProperty( "total_noise" , GWSObjectFactory::incrementValue( existing_value , total_noise ));


    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
