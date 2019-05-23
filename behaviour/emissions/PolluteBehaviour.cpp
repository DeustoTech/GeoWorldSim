#include "PolluteBehaviour.h"
#include <QFile>
#include "../../util/io/csv/CsvImporter.h"
#include "../../skill/pollute/PolluteSkill.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

QString PolluteBehaviour::INPUT_POLLUTANT_TYPES = "input_pollutant_types";
QString PolluteBehaviour::OUTPUT_POLLUTANTS = "output_pollutants";
QString PolluteBehaviour::CURRENT_ROAD_GRADIENT = "input_current_road_gradient";
QString PolluteBehaviour::CURRENT_ROAD_TYPE = "input_current_road_type";
QString PolluteBehaviour::CURRENT_ROAD_TRAFFIC_SITUATION = "input_current_road_traffic_situation";
QString PolluteBehaviour::INPUT_VEHICLE_TYPE = "input_vehicle_type";
QString PolluteBehaviour::INPUT_TRANSPORT_MODE = "input_transport_mode";
QString PolluteBehaviour::ABATEMENT_TYPE = "abatement_type";
QString PolluteBehaviour::NEXTS = "nexts";

PolluteBehaviour::PolluteBehaviour() : GWSBehaviour (){
}

/**********************************************************************
 INITIALIZE
**********************************************************************/

void PolluteBehaviour::afterCreateHook(){

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
    GWSLengthUnit distance = current_coor.getDistance( this->last_position );

    QString transport_mode = this->getProperty( INPUT_TRANSPORT_MODE ).toString();

    QSharedPointer<MoveThroughRouteSkill> movethroughroute_skill = agent->getSkill( MoveThroughRouteSkill::staticMetaObject.className() ).dynamicCast<MoveThroughRouteSkill>();
    QSharedPointer<GWSAgent> current_edge = movethroughroute_skill->getCurrentEdge();

    if ( transport_mode == "WALK" || transport_mode == "BICYCLE" || current_edge.isNull() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
    }

    // DEFAULT VEHICLE TYPES
    QMap< QString , QString > default_vehicles;
    default_vehicles["CAR"] = "PC_diesel_1,4-<2L_Euro-6_DPF";
    default_vehicles["MOTORCYCLE"] = "MC_2S_<=150cc_Euro-2";
    default_vehicles["BUS"] = "UBus_Std_>15-18t_Euro-IV_EGR";
    default_vehicles["ELECTRIC"] = "121.000000inductionLiIon";

    QString vehicle_type = agent->getProperty( INPUT_VEHICLE_TYPE ).toString();

    if ( vehicle_type.isEmpty() ){
        // Get and set vehicle type from default
       vehicle_type = default_vehicles[ transport_mode ];
    }

    double gradient = agent->getProperty( CURRENT_ROAD_GRADIENT ).toDouble();
    double trafficSit = agent->getProperty( CURRENT_ROAD_TRAFFIC_SITUATION ).toDouble();
    GWSSpeedUnit vehicle_speed = agent->getProperty( MoveSkill::CURRENT_SPEED ).toDouble();
    QString roadType = current_edge->getProperty( "type" ).toString().toLower();
     
    QSharedPointer<PolluteSkill> pollute_skill = agent->getSkill( PolluteSkill::staticMetaObject.className() ).dynamicCast<PolluteSkill>();
    QJsonArray pollutant_types = this->getProperty( INPUT_POLLUTANT_TYPES ).toArray();
    
    foreach ( QJsonValue pollutant , pollutant_types ){

         GWSMassUnit emission = pollute_skill->pollute( vehicle_type, transport_mode , pollutant.toString() , vehicle_speed , gradient , roadType , trafficSit , distance );

         if ( !emission.isValid() ){
             emission = 0;
         }
         // Store noise
         agent->setProperty( this->getProperty( OUTPUT_POLLUTANTS ).toString("vehicle_" + pollutant.toString() ) , emission.number() );

    }

    // Save polluted amount:
    /*qDebug() << "Previous total" << agent->getProperty( "total_" + pollutant ).toDouble();
    qDebug() << "Current emission" << emission.number();
    qDebug() << "New total" << agent->getProperty( "total_" + pollutant ).toDouble() + emission.number();*/


    //agent->setProperty( "total_" + pollutant , agent->getProperty( "total_" + pollutant ).toDouble() + emission.number() );
    //agent->setProperty( "current_" + pollutant , emission.number() );

    this->last_position = current_coor;

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
