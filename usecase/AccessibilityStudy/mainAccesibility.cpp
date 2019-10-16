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
#include "../../behaviour/property/CompareEntityPropertyBehaviour.h"
#include "../../behaviour/execution/StopEntityBehaviour.h"
#include "../../behaviour/property/SetEntityPropertyBehaviour.h"
#include "../../behaviour/accessibility/FindDirectClosestBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"
#include "../../behaviour/property/AddGWSGroupPropertyBehaviour.h"
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
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
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
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSEntityEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( FindDirectClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindRoutingClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendEntitySnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CompareEntityPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopEntityBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetEntityPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( AddGWSGroupPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransactionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckPropertyValueBehaviour::staticMetaObject );


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






