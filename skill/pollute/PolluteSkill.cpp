#include "PolluteSkill.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/MoveSkill.h"


QString PolluteSkill::POLLUTANT_TYPE_PROP = "pollutant_type";
QString PolluteSkill::VEHICLE_TYPE_PROP = "vehicle_type";

PolluteSkill::PolluteSkill()  : GWSSkill( ){

}



PolluteSkill::~PolluteSkill(){
}



GWSMassUnit PolluteSkill::pollute( GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit ){

        QSharedPointer<GWSAgent> agent = this->getAgent();

        if ( !this->svm ){

            this->svm = new GWSSvm();
            qDebug() << "Creating svm";

            // Path to HBEFA files:
            QString hbefa_file_path = "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/";

            // Vehicle subtype:
            QString vehicle_subtype = agent->getProperty( "vehicle_subtype" ).toString();

            // Pollutant:
            QString pollutant = agent->getProperty( "pollutant_type" ).toString();

            // SVM trained model:
            QString svm_model = "svm_model";

            // SVR training parameters:
            QString svm_parameters = "model_params";

            // Total paths:
            QString totalPath_model = hbefa_file_path + vehicle_subtype + pollutant + svm_model;
            QString totalPath_params = hbefa_file_path + vehicle_subtype + pollutant + svm_parameters;

            qDebug() << totalPath_model << totalPath_params;

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
        agent->setProperty( "polluted_amount" , agent->getProperty( "polluted_amount").toDouble() + polluted_amount );

        return GWSMassUnit();

}
