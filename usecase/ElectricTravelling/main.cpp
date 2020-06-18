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
#include "../../behaviour/electricTravelling/CalculateETPlannerRouteBehaviour.h"
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
    environment::GridEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    ObjectFactory::globalInstance()->registerType( behaviour::MoveThroughNetworkBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SendEntitySnapshotBehaviour::staticMetaObject);
    ObjectFactory::globalInstance()->registerType( behaviour::SendPropertyStatisticsBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::VehicleNoiseBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::StopEntityBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::CalculateETPlannerRouteBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SetNextRouteDestinationBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::PolluteBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::CheckIfEntitiesRunningBehaviour::staticMetaObject );
    ObjectFactory::globalInstance()->registerType( behaviour::SetPropertyBehaviour::staticMetaObject );

    // CREATE POPULATION
    QList<datasource::EntityGeneratorDatasource*> pending_datasources;  
    QDateTime download_started_time = QDateTime::currentDateTime();

    geoworldsim::datasource::EntityGeneratorDatasource* reader = new geoworldsim::datasource::EntityGeneratorDatasource( app->getConfiguration() );
    reader->connect( reader , &geoworldsim::datasource::EntityGeneratorDatasource::dataReadingFinishedSignal , [ reader , download_started_time ](){

        emit geoworldsim::environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                    QJsonObject({ { "message" , QString("Data download took %1 seconds. Starting execution soon").arg( download_started_time.secsTo( QDateTime::currentDateTime() ) ) } }) , geoworldsim::App::globalInstance()->getAppId() + "-LOG" );

        geoworldsim::environment::ExecutionEnvironment::globalInstance()->run();

    });
    geoworldsim::environment::ExecutionEnvironment::globalInstance()->run();

    // LISTEN TO EXTERNAL SIMULATIONS
    // GWSExternalListener and GWSCommunicationEnvironment have changed, do the code below needs to eventually be modified:
    QJsonObject json_external_listeners = geoworldsim::App::globalInstance()->getConfiguration().value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        if ( !json_external_listeners[ key ].isNull() ){
        }
        qDebug() << QString("Creating external listener %1").arg( key );
     }


    app->exec();

 }
