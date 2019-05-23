#include "PolluteSkill.h"

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../util/storage/GlobalObjectStorage.h"

QString PolluteSkill::POLLUTANT_TYPE_PROP = "pollutant_type";
QString PolluteSkill::VEHICLE_TYPE_PROP = "vehicle_subtype";

PolluteSkill::PolluteSkill()  : GWSSkill( ){

}

PolluteSkill::~PolluteSkill(){
}


GWSMassUnit PolluteSkill::pollute( QString vehicle_type , QString transport_mode, QString pollutant , GWSSpeedUnit speed , double gradient , QString roadType , double trafficSit , GWSLengthUnit distance ){

        QSharedPointer<GWSAgent> agent = this->getAgent();

        GWSMassUnit polluted_amount;

        //QMap < QString, GWSSvm* > models = this->models;
        QString svm_path = "/home/maialen/Escritorio/WorkSpace/MachineLearning/LIBSVMExamples/HBEFA/" + pollutant + "/" + transport_mode + "/" + vehicle_type + "/" + pollutant;
        QSharedPointer<GWSSvm> pollutant_svm = GWSGlobalObjectStorage::globalInstance()->getByName< GWSSvm >( svm_path );

        //QSharedPointer<GWSSvm> pollutant_svm = GWSGlobalObjectStorage::getByName( svm_path );
       // QSharedPointer< GWSObject > getByName( QString name ) const;
        if ( !pollutant_svm ){

           // QString svm_path =  "/home/maialen/Escritorio/WorkSpace/ElectricTravelling/ElectricTravellingImpactTool/data/" + vehicle_type + "/" + pollutant ;

            QDir dir( svm_path );

            if ( dir.exists() ){

                QSharedPointer<GWSSvm> pollutant_svm = QSharedPointer<GWSSvm>(); ;
                pollutant_svm->setObjectName( svm_path );

                QString svm_model = "svm_model";
                QString svm_parameters = "params_model";

                // Total paths:
                QString totalPath_model = svm_path + "/" + svm_model;
                QString totalPath_params = svm_path +  "/" + svm_parameters;

               // Load trained SVM model and parameters:
               //svm->loadTrained( totalPath_model , totalPath_params );
                GWSGlobalObjectStorage::globalInstance()->add( pollutant_svm );

            }
            else {

                qDebug() << svm_path;
                qDebug() << "No SVM for selected subsegment and/or pollutant!";

                polluted_amount = GWSMassUnit( 0 );
                return polluted_amount;
            }
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
