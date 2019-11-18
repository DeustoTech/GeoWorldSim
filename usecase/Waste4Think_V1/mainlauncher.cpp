#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtMath>

// Skills

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/time/WaitUntilTimeBehaviour.h"
#include "../../behaviour/transaction/TransactionBehaviour.h"
#include "../../behaviour/property/ComparePropertyBehaviour.h"
#include "../../behaviour/random/GenerateRandomValueBehaviour.h"
#include "../../behaviour/waste/GenerateWasteBehaviour.h"
#include "../../behaviour/property/SetPropertyBehaviour.h"
#include "../../behaviour/property/MathPropertyBehaviour.h"
#include "../../behaviour/property/AccumulatePropertyBehaviour.h"

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
#include "../../util/io/csv/CsvImporter.h"
#include "../../util/ai/Intelligence.h"
#include "../../util/svm/Svm.h"


int main(int argc, char* argv[])
{

    QDateTime start = QDateTime::currentDateTime();

    // CREATE QAPPLICATION
    geoworldsim::App* app = geoworldsim::App::globalInstance( argc , argv );

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // INIT SINGLETONS
    geoworldsim::ObjectFactory::globalInstance();
    geoworldsim::environment::EntityEnvironment::globalInstance();
    geoworldsim::environment::ExecutionEnvironment::globalInstance();
    geoworldsim::environment::PhysicalEnvironment::globalInstance();
    geoworldsim::environment::NetworkEnvironment::globalInstance();
    geoworldsim::environment::TimeEnvironment::globalInstance();
    geoworldsim::environment::CommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::WaitUntilTimeBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::AccumulatePropertyBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::TransactionBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::ComparePropertyBehaviour::staticMetaObject);
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::GenerateRandomValueBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::GenerateWasteBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::SetPropertyBehaviour::staticMetaObject );
    geoworldsim::ObjectFactory::globalInstance()->registerType( geoworldsim::behaviour::MathPropertyBehaviour::staticMetaObject );

    // CREATE POPULATION
    QDateTime datasource_download_time = QDateTime::currentDateTime();
    geoworldsim::datasource::EntityGeneratorDatasource* reader = new geoworldsim::datasource::EntityGeneratorDatasource( app->getConfiguration() );
    reader->connect( reader , &geoworldsim::datasource::EntityGeneratorDatasource::dataReadingFinishedSignal , [ reader , datasource_download_time ](){

        emit geoworldsim::environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                    QJsonObject({ { "message" , QString("Data download took %1 seconds. Starting execution soon").arg( datasource_download_time.secsTo( QDateTime::currentDateTime() ) ) } }) , geoworldsim::App::globalInstance()->getAppId() + "-LOG" );

        geoworldsim::environment::ExecutionEnvironment::globalInstance()->run();

    });

    // LISTEN TO EXTERNAL SIMULATIONS
    // GWSExternalListener and GWSCommunicationEnvironment have changed, do the code below needs to eventually be modified:
    QJsonObject json_external_listeners = geoworldsim::App::globalInstance()->getConfiguration().value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        if ( !json_external_listeners[ key ].isNull() ){
            new geoworldsim::network::ListenerWebSocket( json_external_listeners[ key ].toString() );
        }
        qDebug() << QString("Creating external listener %1").arg( key );
     }

    app->exec();

}






