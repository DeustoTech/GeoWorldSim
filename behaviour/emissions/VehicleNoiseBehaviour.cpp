#include "VehicleNoiseBehaviour.h"

#include <QtMath>

#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughNetworkSkill.h"
#include "../../object/ObjectFactory.h"

QString geoworldsim::behaviour::VehicleNoiseBehaviour::INPUT_VEHICLE_TYPE = "input_vehicle_type";
QString geoworldsim::behaviour::VehicleNoiseBehaviour::OUTPUT_NOISE = "output_noise";
QString geoworldsim::behaviour::VehicleNoiseBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::VehicleNoiseBehaviour::VehicleNoiseBehaviour() : Behaviour(){

}

QPair<double, QJsonArray> geoworldsim::behaviour::VehicleNoiseBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();
    double A_roll = 0;
    double B_roll = 0;
    double A_prop = 0;
    double B_prop = 0;
    double vref = 70.0;

    unit::SpeedUnit vehicle_speed = agent->getProperty( skill::MoveSkill::INSTANT_SPEED ).toDouble();
    QString vehicle_type = this->getProperty( INPUT_VEHICLE_TYPE ).toString();

    QSharedPointer<skill::MoveThroughNetworkSkill> movethroughroute_skill = agent->getSkill( skill::MoveThroughNetworkSkill::staticMetaObject.className() ).dynamicCast<skill::MoveThroughNetworkSkill>();
    QSharedPointer<Entity> current_edge = movethroughroute_skill->getCurrentEdge();

    double instant_noise = 0;

//    if ( current_edge ){

        if( vehicle_type.toUpper() == "CAR" || vehicle_type.toUpper() == "PC" ){
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


        if( vehicle_type.toUpper() == "MOTORCYCLE" || vehicle_type.toUpper() == "MC" ){
            A_roll = 0;
            B_roll = 0;
            A_prop = 93.2;
            B_prop = 12.6;
        }

        // Rolling Noise:
        double rolling_noise = A_roll + B_roll * log10( vehicle_speed.number() / vref );

        // Propulsion Noise:
        double propulsion_noise = A_prop + B_prop * ( vehicle_speed.number() - vref ) / vref;

        instant_noise = 10 * log10( qPow( 10 , rolling_noise / 10.0 )  +  qPow( 10 , propulsion_noise / 10.0 ) );

//    }

    // Store noise
    agent->setProperty( this->getProperty( OUTPUT_NOISE ).toString( "instant_noise" ) ,  instant_noise );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
