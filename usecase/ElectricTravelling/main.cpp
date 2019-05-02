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
#include "../../util/api/APIDriver.h"


// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/emissions/VehicleNoiseBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/property/PropertyStatisticsBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/electricTravelling/GTAlgBehaviour.h"

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
    GWSAPIDriver::globalInstance();

    // AVAILABLE BEHAVIOURS
    //GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteInVehicleBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( PropertyStatisticsBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( VehicleNoiseBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GTAlgBehaviour::staticMetaObject );


    // CREATE POPULATION
    QList<GWSAgentGeneratorDatasource*> pending_datasources;
    QDateTime datasource_download_time = QDateTime::currentDateTime();
    QJsonObject json_population = GWSApp::globalInstance()->getConfiguration().value("population").toObject();
     foreach( QString key , json_population.keys() ) {

         // Population type:
         QJsonObject population = json_population[ key ].toObject();

         if ( !population.value( "template" ).isNull() && !population.value( "datasources" ).isNull() ){

             QJsonArray datasources = population.value( "datasources" ).toArray();

             for ( int i = 0; i <  datasources.size() ; ++i ){

                 QJsonObject datasource = datasources.at( i ).toObject();
                 QString scenario_id = datasource.value("scenario_id").toString();
                 int limit = datasource.value("limit").toInt(-1);
                 QString entity_type = datasource.value("entity_type").toString();
                 if( scenario_id.isEmpty() || entity_type.isEmpty() ){
                     qWarning() << "Asked to download from scenario without ID or entity_type";
                 }

                 GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value( "template" ).toObject() , scenario_id,  entity_type , limit > 0 ? limit : 999999999999999 );
                 pending_datasources.append( ds );

                 ds->connect( ds , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [ ds , &pending_datasources , datasource_download_time ](){
                     pending_datasources.removeAll( ds );
                     ds->deleteLater();
                     if( pending_datasources.isEmpty() ){
                         qDebug() << "Elapsed time" << QDateTime::currentDateTime().secsTo( datasource_download_time );
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
     QJsonObject json_external_listeners = GWSApp::globalInstance()->getConfiguration().value("external_listeners").toObject();
     foreach( QString key , json_external_listeners.keys() ) {

         // Get simulation to be listened to from config.json file
         if ( !json_external_listeners[ key ].isNull() ){
             new GWSExternalListener( json_external_listeners[ key ].toString() );
         }
         qDebug() << QString("Creating external listener %1").arg( key );
      }


     app->exec();

 }






