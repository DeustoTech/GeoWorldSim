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
#include "../../entity/Entity.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"

// Utils
#include "../../util/parallelism/ParallelismController.h"
#include "../../util/geometry/Coordinate.h"
//#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/EntityGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/io/csv/CsvImporter.h"
#include "../../util/ai/Intelligence.h"
#include "../../util/svm/Svm.h"
#include "../../util/network/ListenerWebSocket.h"


// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughNetworkBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/emissions/VehicleNoiseBehaviour.h"
#include "../../behaviour/information/SendEntitySnapshotBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/property/SetPropertyBehaviour.h"
#include "../../behaviour/property/SendPropertyStatisticsBehaviour.h"
#include "../../behaviour/execution/CheckIfEntitiesRunningBehaviour.h"
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.h"
#include "../../behaviour/move/SetNextRouteDestinationBehaviour.h"
#include "../../behaviour/emissions/PolluteBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"

using namespace geoworldsim;

int main(int argc, char* argv[])
{

    QDateTime start = QDateTime::currentDateTime();

    // CREATE QAPPLICATION
    App* app = geoworldsim::App::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    ObjectFactory::globalInstance();
    parallel::ParallelismController::globalInstance();
    environment::EntityEnvironment::globalInstance();
    environment::ExecutionEnvironment::globalInstance();
    environment::PhysicalEnvironment::globalInstance();
    environment::NetworkEnvironment::globalInstance();
    environment::TimeEnvironment::globalInstance();
    environment::CommunicationEnvironment::globalInstance();
    environment::GWSGridEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    ObjectFactory::globalInstance()->registerType( behaviour::MoveThroughNetworkBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SendEntitySnapshotBehaviour::staticMetaObject);
    ObjectFactory::globalInstance()->registerType( behaviour::SendPropertyStatisticsBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::VehicleNoiseBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::StopEntityBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::CalculateGTAlgRouteBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SetNextRouteDestinationBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::PolluteBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::CheckIfEntitiesRunningBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SetPropertyBehaviour::staticMetaObject );

    // CREATE POPULATION
    QList<datasource::EntityGeneratorDatasource*> pending_datasources;
    QDateTime datasource_download_time = QDateTime::currentDateTime();
    QJsonObject json_population = App::globalInstance()->getConfiguration().value("population").toObject();
    foreach( QString key , json_population.keys() ) {

         // Population type:
         QJsonObject population = json_population[ key ].toObject();

         if ( !population.value( "template" ).isNull() && !population.value( "datasources" ).isNull() ){

             QJsonArray datasources = population.value( "datasources" ).toArray();

             for ( int i = 0; i <  datasources.size() ; ++i ){

                 QJsonObject datasource = datasources.at( i ).toObject();
                 QString scenario_id = datasource.value("scenario_id").toString();
                 QString user_id = datasource.value("user_id").toString();
                 int limit = datasource.value("limit").toInt(-1);
                 QString entity_type = datasource.value("entity_type").toString();
                 QString entity_filter = datasource.value("entity_filter").toString();
                 if( scenario_id.isEmpty() || entity_type.isEmpty() ){
                     qWarning() << "Asked to download from scenario without ID or entity_type";
                 }                                                                                                        
                 datasource::EntityGeneratorDatasource* ds = new datasource::EntityGeneratorDatasource( population.value( "template" ).toObject() ,user_id , scenario_id,  entity_type , entity_filter , limit > 0 ? limit : 999999999999999 );
                 pending_datasources.append( ds );

                 ds->connect( ds , &datasource::EntityGeneratorDatasource::dataReadingFinishedSignal , [ ds , &pending_datasources , datasource_download_time ](){
                     pending_datasources.removeAll( ds );
                     ds->deleteLater();
                     if( pending_datasources.isEmpty() ){

                         emit environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                                     QJsonObject({ { "message" , QString("Data download took %1 seconds. Starting execution soon").arg( datasource_download_time.secsTo( QDateTime::currentDateTime() ) ) } }) , App::globalInstance()->getAppId() + "-LOG" );

                         environment::ExecutionEnvironment::globalInstance()->run();
                     }
                 });

             }
         }

         if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
             for ( int i = 0; i < population.value("amount").toInt() ; i++){
                 // Use template to generate amount entities
                 ObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<Entity>();
             }
         }
        qInfo() << QString("Creating population %1").arg( key );
     }
     if( pending_datasources.isEmpty() ){

         emit environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                     QJsonObject({ { "message" , QString("No data to download. Starting execution soon") } }) , App::globalInstance()->getAppId() + "-LOG" );


         environment::ExecutionEnvironment::globalInstance()->run();
     }

     // LISTEN TO EXTERNAL SIMULATIONS
     // GWSExternalListener and environment::CommunicationEnvironment have changed, do the code below needs to eventually be modified:
     QJsonObject json_external_listeners = App::globalInstance()->getConfiguration().value("external_listeners").toObject();
     foreach( QString key , json_external_listeners.keys() ) {

         // Get simulation to be listened to from config.json file
         if ( !json_external_listeners[ key ].isNull() ){
             new network::ListenerWebSocket( json_external_listeners[ key ].toString() );
         }
         qDebug() << QString("Creating external listener %1").arg( key );
      }

     app->exec();

 }






