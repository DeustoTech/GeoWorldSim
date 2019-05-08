#include "PolluteSkill.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/MoveSkill.h"


QString PolluteSkill::POLLUTANT_TYPE_PROP = "pollutant_type";
QString PolluteSkill::VEHICLE_TYPE_PROP = "vehicle_subtype";

PolluteSkill::PolluteSkill()  : GWSSkill( ){

}

PolluteSkill::~PolluteSkill(){
}


GWSMassUnit PolluteSkill::pollute( QString vehicle_type , QString transport_mode, QString pollutant , GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit , GWSLengthUnit distance ){

        QSharedPointer<GWSAgent> agent = this->getAgent();

        GWSMassUnit polluted_amount;

        QMap < QString, GWSSvm* > models = this->models;

        if ( !models.keys( ).contains( pollutant ) ){

            models.insert( pollutant , new GWSSvm() );

        }

        GWSSvm* pollutant_svm = models.value( pollutant );

        QString svm_path = "/home/maialen/Escritorio/WorkSpace/MachineLearning/LIBSVMExamples/HBEFA/" + pollutant + "/" + transport_mode + "/" + vehicle_type + "/" + pollutant;
       // QString svm_path =  "/home/maialen/Escritorio/WorkSpace/ElectricTravelling/ElectricTravellingImpactTool/data/" + vehicle_type + "/" + pollutant ;

        QDir dir( svm_path );

        if ( dir.exists() ){

            QString svm_model = "svm_model";
            QString svm_parameters = "params_model";


            // Total paths:
            QString totalPath_model = svm_path + "/" + svm_model;
            QString totalPath_params = svm_path +  "/" + svm_parameters;


           // Load trained SVM model and parameters:
            pollutant_svm->loadTrained( totalPath_model , totalPath_params );

        }

        else {
            qDebug() << "No SVM for selected subsegment and/or pollutant!";

            polluted_amount = GWSMassUnit( 0 );
            return polluted_amount;
        }



        QMap<QString , QVariant> input;

        // Extract current speed from agent:
        input.insert( "velocity" , speed.value );
        input.insert( "gradient" , gradient );
        input.insert( "roadType" , roadType);
        input.insert( "trafficSit" , 0.36);

        QJsonObject result = pollutant_svm->run( input );
        polluted_amount = result.value( "EFA" ).toDouble() * distance.number() / 1000.;// Because the SVM outputs emissions per km

        // Save polluted amount:
        // agent->setProperty( "total"+ pollutant +"amount" , agent->getProperty( "total"+ pollutant +"amount").toDouble() + polluted_amount );

        return polluted_amount;



}
