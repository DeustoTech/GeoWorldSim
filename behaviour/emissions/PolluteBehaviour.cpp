#include "PolluteBehaviour.h"
#include <QFile>
#include "../../util/io/csv/CsvImporter.h"
#include "../../skill/pollute/PolluteSkill.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString PolluteBehaviour::POLLUTANT_TYPE = "pollutant_type";
//QString PolluteBehaviour::STORE_POLLUTANT_VALUE_AS = "store_pollutant_value_as";
QString PolluteBehaviour::CURRENT_ROAD_GRADIENT = "current_road_gradient";
QString PolluteBehaviour::CURRENT_ROAD_TYPE = "current_road_type";
QString PolluteBehaviour::CURRENT_ROAD_TRAFFIC_SITUATION = "current_road_traffic_situation";
QString PolluteBehaviour::VEHICLE_TYPE = "vehicle_type";
QString PolluteBehaviour::TRANSPORT_MODE = "transport_mode";
QString PolluteBehaviour::ABATEMENT_TYPE = "abatement_type";
QString PolluteBehaviour::NEXTS = "nexts";

PolluteBehaviour::PolluteBehaviour() : GWSBehaviour (){
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
    GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    GWSCoordinate current_coor = agent_geom.getCentroid();

    QString vehicle_type = agent->getProperty( VEHICLE_TYPE  ).toString();
    //QString vehicle_type = "MC_2S_<=150cc_Euro-2";

    QString transport_mode = agent->getProperty( TRANSPORT_MODE ).toString();

    if ( transport_mode == "WALK" || transport_mode == "BICYCLE" ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    //QString abatement = agent->getProperty( this->getProperty( ABATEMENT_TYPE ).toString() ).toString();
    GWSSpeedUnit vehicle_speed = this->getProperty( MoveSkill::STORE_CURRENT_SPEED_PROP ).toDouble();

    //QString pollutant = "CO2";
    QString pollutant = this->getProperty( POLLUTANT_TYPE ).toString();

    double gradient = agent->getProperty( CURRENT_ROAD_GRADIENT ).toDouble();
    QString roadType = agent->getProperty( MoveThroughRouteSkill::STORE_CURRENT_ROAD_TYPE ).toString().toLower();
    double trafficSit = agent->getProperty( CURRENT_ROAD_TRAFFIC_SITUATION ).toDouble();

    QSharedPointer<PolluteSkill> pollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();



    GWSLengthUnit distance = current_coor.getDistance( this->last_position );

    GWSMassUnit emission = pollute_skill->pollute( vehicle_type , transport_mode , pollutant , vehicle_speed , gradient , roadType , trafficSit , distance );

    if ( !emission.isValid() ){

        emission = 0;
    }

    // Save polluted amount:
    /*qDebug() << "Previous total" << agent->getProperty( "total_" + pollutant ).toDouble();
    qDebug() << "Current emission" << emission.number();
    qDebug() << "New total" << agent->getProperty( "total_" + pollutant ).toDouble() + emission.number();*/


    agent->setProperty( "total_" + pollutant , agent->getProperty( "total_" + pollutant ).toDouble() + emission.number() );
    agent->setProperty( "current_" + pollutant , emission.number() );

    this->last_position = current_coor;

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
