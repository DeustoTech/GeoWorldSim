#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtMath>

#include "../../entity/Entity.h"


// Skills
#include "../../skill/Skill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/information/SendEntitySnapshotBehaviour.h"
#include "../../behaviour/property/ComparePropertyBehaviour.h"
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/property/SetPropertyBehaviour.h"
#include "../../behaviour/accessibility/FindDirectClosestBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"
#include "../../behaviour/transaction/TransactionBehaviour.h"
#include "../../behaviour/property/CheckPropertyValueBehaviour.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/EntityGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/routing/Routing.h"
#include "../../util/io/csv/CsvImporter.h"
#include "../../util/ai/Intelligence.h"
#include "../../util/svm/Svm.h"



int main(int argc, char* argv[])
{

    // CREATE QAPPLICATION
    geoworldsim::App* app = geoworldsim::App::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    geoworldsim::ObjectFactory::globalInstance();
    geoworldsim::environment::EntityEnvironment::globalInstance();
    geoworldsim::environment::ExecutionEnvironment::globalInstance();
    geoworldsim::environment::PhysicalEnvironment::globalInstance();
    geoworldsim::environment::NetworkEnvironment::globalInstance();
    geoworldsim::environment::TimeEnvironment::globalInstance();
    geoworldsim::environment::CommunicationEnvironment::globalInstance();


    // AVAILABLE BEHAVIOURS
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::FindDirectClosestBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::FindRoutingClosestBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::ComparePropertyBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::StopEntityBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::SetPropertyBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::TransactionBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::CheckPropertyValueBehaviour::staticMetaObject );


    // CREATE POPULATION
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






