#include "PolluteSkill.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/MoveSkill.h"


QString PolluteSkill::POLLUTANT_TYPE_PROP = "pollutant_type";
QString PolluteSkill::VEHICLE_TYPE_PROP = "vehicle_subtype";

PolluteSkill::PolluteSkill()  : GWSSkill( ){

}



PolluteSkill::~PolluteSkill(){
}



GWSMassUnit PolluteSkill::pollute( QString vehicle_type , QString pollutant , GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit ){

        QSharedPointer<GWSAgent> agent = this->getAgent();

        if ( !this->svm ){

            this->svm = new GWSSvm();

            // Path to HBEFA files:
            QString hbefa_file_path = "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/";
            QString svm_model = "svm_model";
            QString svm_parameters = "model_params";

            // Total paths:
            QString totalPath_model = hbefa_file_path + vehicle_type + "/" + pollutant + "/" + svm_model;
            QString totalPath_params = hbefa_file_path + vehicle_type + "/" + pollutant  + "/" + svm_parameters;

           // Load trained SVM model and parameters:
            this->svm->loadTrained( totalPath_model , totalPath_params );

        }

        QMap<QString , QVariant> input;

        // Extract current speed from agent:
        input.insert( "V" , speed.value );
        input.insert("Gradient" , gradient );
        input.insert( "Road_type" , roadType);
        input.insert("Traffic_sit" , 0.36);

        QJsonObject result = this->svm->run( input );

        double polluted_amount = result.value( "EFA" ).toDouble();

        // Save polluted amount:
        agent->setProperty( "total"+ pollutant +"amount" , agent->getProperty( "total"+ pollutant +"amount").toDouble() + polluted_amount );

        return GWSMassUnit();

}
