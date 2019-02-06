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

            // Path to SVM trained files:
            QString svm_path =  "/home/maialen/Escritorio/WorkSpace/ElectricTravelling/ElectricTravellingImpactTool/data/" + vehicle_type + "/" + pollutant ;


            QDir dir( svm_path );
            if ( dir.exists() ){

                QString svm_model = "svm_model";
                QString svm_parameters = "params_model";

                // Total paths:
                QString totalPath_model = svm_path + "/" + svm_model;
                QString totalPath_params = svm_path +  "/" + svm_parameters;


                qDebug() << totalPath_model;
               // Load trained SVM model and parameters:
                this->svm->loadTrained( totalPath_model , totalPath_params );

            }

            else { qDebug() << "No SVM for selected subsegment and/or pollutant!";}



        }

        QMap<QString , QVariant> input;

        // Extract current speed from agent:
        input.insert( "velocity" , speed.value );
        input.insert( "gradient" , gradient );
        input.insert( "roadType" , roadType);
        input.insert( "trafficSit" , 0.36);

        QJsonObject result = this->svm->run( input );
        double polluted_amount = result.value( "EFA" ).toDouble();

        // Save polluted amount:
        agent->setProperty( "total"+ pollutant +"amount" , agent->getProperty( "total"+ pollutant +"amount").toDouble() + polluted_amount );
        return GWSMassUnit();

}
