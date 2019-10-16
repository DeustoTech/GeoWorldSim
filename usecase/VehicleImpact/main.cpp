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
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/move/MoveThroughPathBehaviour.h"
#include "../../behaviour/emissions/PolluteBehaviour.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

// Utils
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/EntityGeneratorDatasource.h"

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
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::MoveThroughPathBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::PolluteBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::StopEntityBehaviour::staticMetaObject );

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






