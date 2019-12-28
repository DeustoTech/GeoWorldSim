#include "PolluteBehaviour.h"
#include <QFile>
#include "../../util/io/csv/CsvImporter.h"
#include "../../skill/pollute/PolluteSkill.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../skill/move/MoveSkill.h"
#include "../../object/ObjectFactory.h"


QString geoworldsim::behaviour::PolluteBehaviour::INPUT_POLLUTANT_TYPES = "input_pollutant_types";
QString geoworldsim::behaviour::PolluteBehaviour::INPUT_CURRENT_ROAD_GRADIENT = "input_current_road_gradient";
QString geoworldsim::behaviour::PolluteBehaviour::INPUT_CURRENT_ROAD_TYPE = "input_current_road_type";
QString geoworldsim::behaviour::PolluteBehaviour::INPUT_CURRENT_ROAD_TRAFFIC_SITUATION = "input_current_road_traffic_situation";
QString geoworldsim::behaviour::PolluteBehaviour::INPUT_VEHICLE_SUBTYPE = "input_vehicle_subtype";
QString geoworldsim::behaviour::PolluteBehaviour::INPUT_VEHICLE_TYPE = "input_vehicle_type";
QString geoworldsim::behaviour::PolluteBehaviour::OUTPUT_POLLUTANTS = "output_pollutants";
QString geoworldsim::behaviour::PolluteBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::PolluteBehaviour::PolluteBehaviour() : Behaviour (){
}

/**********************************************************************
 INITIALIZE
**********************************************************************/

void geoworldsim::behaviour::PolluteBehaviour::afterCreateHook(){

    QSharedPointer< Entity > entity = this->getEntity();

    // Check if agent has a MoveSkill, otherwise create it and set its max_speed
    QSharedPointer< skill::PolluteSkill > pollute_skill = entity->getSkill( skill::PolluteSkill::staticMetaObject.className() ).dynamicCast< skill::PolluteSkill >();
    if( pollute_skill.isNull() ){
        pollute_skill = QSharedPointer< skill::PolluteSkill >( new skill::PolluteSkill() );
        entity->addSkill( pollute_skill );
    }
}


QPair<double, QJsonArray> geoworldsim::behaviour::PolluteBehaviour::behave(){

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

    QSharedPointer< Entity > entity = this->getEntity();
    geometry::Geometry entity_geom = geometry::Geometry( entity->getProperty( geoworldsim::environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
    geometry::Coordinate current_coor = entity_geom.getCentroid();
    unit::LengthUnit distance = current_coor.getDistance( this->last_position );

    QString transport_mode = this->getProperty( INPUT_VEHICLE_TYPE ).toString();

    // DEFAULT VEHICLE TYPES
    QMap< QString , QString > default_vehicles;
    default_vehicles["CAR"] = "PC_diesel_1,4-<2L_Euro-6_DPF";
    default_vehicles["MOTORCYCLE"] = "MC_2S_<=150cc_Euro-2";
    default_vehicles["BUS"] = "UBus_Std_>15-18t_Euro-IV_EGR";
    default_vehicles["ELECTRIC"] = "121.000000inductionLiIon";

    QString vehicle_subtype = entity->getProperty( INPUT_VEHICLE_SUBTYPE ).toString();

    if ( vehicle_subtype.isEmpty() ){
        // Get and set vehicle type from default
       vehicle_subtype = default_vehicles[ transport_mode ];
    }

    double gradient = entity->getProperty( INPUT_CURRENT_ROAD_GRADIENT ).toDouble();
    double trafficSit = entity->getProperty( INPUT_CURRENT_ROAD_TRAFFIC_SITUATION ).toDouble();
    unit::SpeedUnit vehicle_speed = entity->getProperty( skill::MoveSkill::INSTANT_SPEED ).toDouble();
    QString road_type = entity->getProperty( INPUT_CURRENT_ROAD_TYPE ).toString().toLower();
     
    QSharedPointer< skill::PolluteSkill > pollute_skill = entity->getSkill( skill::PolluteSkill::staticMetaObject.className() ).dynamicCast< skill::PolluteSkill >();
    QJsonArray pollutant_types = this->getProperty( INPUT_POLLUTANT_TYPES ).toArray();
    
    foreach ( QJsonValue pollutant , pollutant_types ){

         unit::MassUnit emission = pollute_skill->pollute( transport_mode , vehicle_subtype, pollutant.toString() , vehicle_speed , gradient , road_type , trafficSit , distance );

         if ( !emission.isValid() ){
             emission = 0;
         }

         // Store value
         double existing_value = entity->getProperty("accumulated_" + pollutant.toString() ).toDouble();
         entity->setProperties( QJsonObject({
            { "instant_" + pollutant.toString() , emission.number() },
            { "accumulated_" +  pollutant.toString() , ObjectFactory::incrementValue( existing_value , emission.number() ) }
            }) );

    }

    this->last_position = current_coor;

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
