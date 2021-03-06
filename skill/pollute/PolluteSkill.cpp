#include "PolluteSkill.h"

#include <QDir>
#include <QJsonDocument>

#include "../../util/storage/GlobalObjectStorage.h"

QString geoworldsim::skill::PolluteSkill::POLLUTANT_TYPE_PROP = "pollutant_type";
QString geoworldsim::skill::PolluteSkill::VEHICLE_TYPE_PROP = "vehicle_subtype";

geoworldsim::skill::PolluteSkill::PolluteSkill() : Skill( ){

}

geoworldsim::skill::PolluteSkill::~PolluteSkill(){
}


geoworldsim::unit::MassUnit geoworldsim::skill::PolluteSkill::pollute( QString transport_mode, QString vehicle_subtype , QString pollutant , geoworldsim::unit::SpeedUnit speed , double gradient , QString roadType , double trafficSit , geoworldsim::unit::LengthUnit distance ){

        QSharedPointer<Entity> agent = this->getEntity();

        unit::MassUnit polluted_amount;

        //QMap < QString, GWSSvm* > models = this->models;
        //QString svm_path = "/home/maialen/Escritorio/WorkSpace/MachineLearning/LIBSVMExamples/HBEFA/" + pollutant + "/" + transport_mode + "/" + vehicle_type + "/" + pollutant;
        QString svm_path = "../../../data/" + transport_mode + "/" + vehicle_subtype + "/" + pollutant;
        QSharedPointer< intelligence::Svm > pollutant_svm = storage::GlobalObjectStorage::globalInstance()->getByName< intelligence::Svm >( svm_path );

        if ( !pollutant_svm ){

            pollutant_svm = QSharedPointer<intelligence::Svm>( new intelligence::Svm() );
            pollutant_svm->setObjectName( svm_path );

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

                qWarning() << QString("Could not find SVM file %1").arg( svm_path );
            }

            storage::GlobalObjectStorage::globalInstance()->add( pollutant_svm );
        }

        QMap<QString , QVariant> input;

        // Extract current speed from agent:
        input.insert( "velocity" , speed.value );
        input.insert( "gradient" , gradient );
        input.insert( "roadType" , roadType );
        input.insert( "trafficSit" , 0.36 );

        QJsonObject result = pollutant_svm->run( input );
        polluted_amount = result.value( "EFA" ).toDouble( 0 ) * distance.number() / 1000.;// Because the SVM outputs emissions per km

        return polluted_amount;

}
