#include "PolluteBehaviour.h"
#include <QFile>
#include "../../util/io/csv/CsvImporter.h"
#include "../../skill/pollute/PolluteSkill.h"
#include "../../skill/move/MoveSkill.h"

QString PolluteBehaviour::POLLUTANT_TYPE = "pollutant_type";
//QString PolluteBehaviour::STORE_POLLUTANT_VALUE_AS = "store_pollutant_value_as";
QString PolluteBehaviour::CURRENT_ROAD_GRADIENT = "current_road_gradient";
QString PolluteBehaviour::CURRENT_ROAD_TYPE = "current_road_type";
QString PolluteBehaviour::CURRENT_ROAD_TRAFFIC_SITUATION = "current_road_traffic_situation";
QString PolluteBehaviour::VEHICLE_TYPE = "vehicle_type";
//QString PolluteBehaviour::VEHICLE_SUBTYPE = "vehicle_subtype";
QString PolluteBehaviour::ABATEMENT_TYPE = "abatement_type";
QString PolluteBehaviour::NEXTS = "nexts";

PolluteBehaviour::PolluteBehaviour() : GWSBehaviour ()
{


}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void PolluteBehaviour::initialize(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer<PolluteSkill> pollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();
    if( pollute_skill.isNull() ){
        pollute_skill = QSharedPointer<PolluteSkill>( new PolluteSkill() );
        agent->addSkill( pollute_skill );
    }
}


QPair< double , QJsonArray >  PolluteBehaviour::behave(){


    QSharedPointer<GWSAgent> agent = this->getAgent();

    //QString vehicle_type = agent->getProperty( this->getProperty( VEHICLE_TYPE ).toString() ).toStr
    QString vehicle_type = "MC_2S_<=150cc_Euro-2";

    //QString abatement = agent->getProperty( this->getProperty( ABATEMENT_TYPE ).toString() ).toString();
    GWSSpeedUnit vehicle_speed = this->getProperty( MoveSkill::STORE_CURRENT_SPEED_PROP ).toDouble();

    QString pollutant = "CO2";
    double gradient = agent->getProperty( CURRENT_ROAD_GRADIENT ).toDouble();
    QString roadType = agent->getProperty( CURRENT_ROAD_TYPE ).toString();
    double trafficSit = agent->getProperty( CURRENT_ROAD_TRAFFIC_SITUATION ).toDouble();

    QSharedPointer<PolluteSkill> pollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();

    GWSMassUnit emission = pollute_skill->pollute( vehicle_type , pollutant , vehicle_speed , gradient , roadType , trafficSit );

    // Save polluted amount:
    agent->setProperty( pollutant , agent->getProperty( pollutant ).toDouble() + emission.number() );

   return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
