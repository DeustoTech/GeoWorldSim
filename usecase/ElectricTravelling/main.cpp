#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QProcess>
#include <QtMath>


#include "../../app/App.h"
#include "../../object/ObjectFactory.h"


#include "../../agent/Agent.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/AgentGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/io/csv/CsvImporter.h"
#include "../../util/ai/Intelligence.h"
#include "../../util/svm/Svm.h"
#include "../../util/parallelism/ParallelismController.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/waste4think/DelayBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/waste4think/FindClosestBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/waste4think/FollowTSPRouteBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteInVehicleBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/waste4think/GatherAgentPropertyBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/waste4think/CheckPropertyValueBehaviour.h"
#include "../../behaviour/waste4think/ChooseRandomValueFromSetBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/electricTravelling/DriveBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/waste4think/GenerateRandomValueBehaviour.h"

int main(int argc, char* argv[])
{

    QDateTime start = QDateTime::currentDateTime();

    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteInVehicleBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject ) ;
    GWSObjectFactory::globalInstance()->registerType( GenerateRandomValueBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( DriveBehaviour::staticMetaObject );


    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    // Read agents according to CONFIG file
    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // READ CONFIGURATION
    QFile file( app->property( "config" ).toString() );
    QJsonObject json_configuration;
    if( !file.open( QFile::ReadOnly ) ){
        qCritical() << QString("No configuration file found");
        return -1;
    }
    QJsonParseError jerror;
    json_configuration = QJsonDocument::fromJson( file.readAll() , &jerror ).object();
    if( jerror.error != QJsonParseError::NoError ){
        qCritical() << QString("Error when parsing configuration JSON: %1").arg( jerror.errorString() );
        return -1;
    }

    // CREATE POPULATION
    QList<GWSAgentGeneratorDatasource*> pending_datasources;
    QJsonObject json_population = json_configuration.value("population").toObject();
     foreach( QString key , json_population.keys() ) {

        // Population type:
        QJsonObject population = json_population[ key ].toObject();

        if ( !population.value("template").isNull() && !population.value("datasource_urls").isNull() ){

            QJsonArray datasources = population.value("datasource_urls").toArray();

            for ( int i = 0; i <  datasources.size(); ++i){

                QString ds_url = datasources.at(i).toString();

                GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value("template").toObject() ,  ds_url );
                pending_datasources.append( ds );

                ds->connect( ds , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [ ds , &pending_datasources ](){
                    pending_datasources.removeAll( ds );
                    ds->deleteLater();
                    if( pending_datasources.isEmpty() ){
                        GWSExecutionEnvironment::globalInstance()->run();
                    }
                });

            }


        }

        if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
            for ( int i = 0; i < population.value("amount").toInt() ; i++){
                // Use template to generate amount agents
                GWSObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<GWSAgent>();
            }
        }
       qDebug() << QString("Creating population %1").arg( key );
    }
    if( pending_datasources.isEmpty() ){
        GWSExecutionEnvironment::globalInstance()->run();
    }




    // LISTEN TO EXTERNAL SIMULATIONS
    // GWSExternalListener and GWSCommunicationEnvironment have changed, do the code below needs to eventually be modified:
    QJsonObject json_external_listeners = json_configuration.value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        if ( !json_external_listeners[ key ].isNull() ){
            new GWSExternalListener( json_external_listeners[ key ].toString() );
        }
        qDebug() << QString("Creating external listener %1").arg( key );
     }


    // SVM

    // Loop over all vehicle subtype directories and components
    QString svm_splitted_file_path = "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/";
    QDir directory( svm_splitted_file_path );

    // Vehicle subtype level:
    QFileInfoList vehicleSubsegmentsInfo = directory.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs );
    /*foreach( QFileInfo subsegment, vehicleSubsegmentsInfo) {

        if ( subsegment.isDir() ){

            QFileInfoList pollutionComponentsInfo = QDir( subsegment.absoluteFilePath() ).entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs );

            foreach( QFileInfo component, pollutionComponentsInfo) {

                if ( component.isDir() ){

                    QFileInfoList pollutantFilesInfo = QDir( component.absoluteFilePath() ).entryInfoList( QDir::NoDotAndDotDot | QDir::Files);

                    QString inputs_file_path;
                    QString outputs_file_path;

                    foreach( QFileInfo pollutantFiles, pollutantFilesInfo) {

                        if ( pollutantFiles.isFile() ){
                            qDebug() << pollutantFiles.absoluteFilePath();
                            QString fileName = pollutantFiles.fileName();

                            if ( fileName == "INPUT.csv"  ){
                                qDebug() << "FOUND INPUTS FILE!";
                                inputs_file_path = pollutantFiles.absoluteFilePath();
                            }

                            if ( fileName == "OUTPUT.csv"  ){
                                qDebug() << "FOUND OUTPUTS FILE!";
                                outputs_file_path = pollutantFiles.absoluteFilePath();
                            }

                        }

                    }

                    GWSSvm* svm = new GWSSvm();
                    svm->trainFromFile( inputs_file_path , outputs_file_path );
                    svm->saveTrained( component.absoluteFilePath() + "/svm_model" , component.absoluteFilePath() + "/model_params");

                    svm->deleteLater();

                }
            }
        }

    }*/


        // GWSSvm* svm = new GWSSvm();
        // svm->loadTrained( "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/MC 2S <=150cc Euro-3/CO/svm_model" , "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/MC 2S <=150cc Euro-3/CO/model_params" );
        //QList<QPair < QString, QVariant> > test_input;
         /* test_input = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/St+Go"),
                         QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S 251-750cc Euro-5") ,
                         QPair<QString , QVariant>("V" , 28.1581172943) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};
            */


         // 0;98.7671890259;RUR/Semi-MW/110/Freeflow
       /*  QMap<QString , QVariant> input;
          input.insert("Gradient" , 0);
          input.insert("V" , 98.7671890259);
          input.insert( "Road_type" , "MW");
          input.insert("Traffic_sit" , 0.66);
          qDebug() << svm->run( input );
          return 0;*/
    //}*/


          GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
              QDateTime current_time = QDateTime::currentDateTime();
              qint64 secondsDiff = start.secsTo( current_time );
              qDebug() << "Elapsed time" << secondsDiff;
          });

          app->exec();

}
