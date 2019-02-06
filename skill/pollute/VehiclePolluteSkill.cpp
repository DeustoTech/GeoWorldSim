#include "VehiclePolluteSkill.h"

//QString VehiclePolluteSkill::SKILL_VEHICLE_TYPE_PROP = "vehicle_type";


VehiclePolluteSkill::VehiclePolluteSkill() : PolluteSkill()
{

}


VehiclePolluteSkill::~VehiclePolluteSkill(){
}


GWSMassUnit VehiclePolluteSkill::pollute( GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit ){

    //QSharedPointer<GWSAgent> agent = this->getAgent();

    // Load trained SVM model and parameters:
    this->svm->loadTrained( "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/MC 2S <=150cc Euro-3/CO/svm_model" , "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/MC 2S <=150cc Euro-3/CO/model_params" );

    QMap<QString , QVariant> input;

    input.insert("Gradient" , 0);

    // Extract current speed from agent:
    //double speed = agent->getProperty( MoveSkill::AGENT_CURRENT_SPEED_PROP ).toDouble();
    input.insert("V" , 98.7671890259);

    // Extract current road type from agent:
    // QString currentRoad = agent->getProperty( AGENT_CURRENT_ROAD_TYPE_PROP ).toString();
    input.insert( "Road_type" , "MW");
    input.insert("Traffic_sit" , 0.36);
   // qDebug() << "Pollute" << svm->run( input );

    return GWSMassUnit();
}
