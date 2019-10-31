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
#include "../../behaviour/property/SetPropertyBehaviour.h".h"
#include "../../behaviour/property/MathPropertyBehaviour.h"

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
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
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
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // INIT OBJECT FACTORY
    GWSObjectFactory::globalInstance()->registerType( GWSEntity::staticMetaObject );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSEntityEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( WaitUntilTimeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransactionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ComparePropertyBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( GenerateRandomValueBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MathPropertyBehaviour::staticMetaObject );

    // CREATE POPULATION
    QDateTime datasource_download_time = QDateTime::currentDateTime();
    GWSEntityGeneratorDatasource* reader = new GWSEntityGeneratorDatasource( app->getConfiguration() );
    reader->connect( reader , &GWSEntityGeneratorDatasource::dataReadingFinishedSignal , [ reader , datasource_download_time ](){

        emit GWSCommunicationEnvironment::globalInstance()->sendMessageSignal(
                    QJsonObject({ { "message" , QString("Data download took %1 seconds. Starting execution soon").arg( datasource_download_time.secsTo( QDateTime::currentDateTime() ) ) } }) , GWSApp::globalInstance()->getAppId() + "-LOG" );

        GWSExecutionEnvironment::globalInstance()->run();

    });

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






