#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QtMath>

#include "../../agent/Agent.h"

// Agents



// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

// Behaviours
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/property/IncrementPropertyBehaviour.h"
#include "../../behaviour/agent/FindRandomAgentBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/property/SetAgentPropertyBehaviour.h"


//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/AgentGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/io/csv/CsvImporter.h"

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

    // AVAILABLE AGENTS

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( FindRandomAgentBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransferAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetAgentPropertyBehaviour::staticMetaObject );

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
    int pending_population = 0;
    QJsonObject json_population = json_configuration.value("population").toObject();
    foreach( QString key , json_population.keys() ) {

        // Population type:
        QJsonObject population = json_population[ key ].toObject();

        if ( !population.value("template").isNull() && !population.value("datasource_url").isNull() ){
            pending_population++;
            GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value("template").toObject() , population.value("datasource_url").toString() );
            ds->connect( ds , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [ds,&pending_population](){
                pending_population--;
                ds->deleteLater();
                if( pending_population <= 0 ){
                    GWSExecutionEnvironment::globalInstance()->run();
                }
            });
        }

        if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
            for ( int i = 0; i < population.value("amount").toInt() ; i++){
                // Use template to generate amount agents
                GWSObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<GWSAgent>();
            }
        }
       qDebug() << QString("Creating population %1").arg( key );
    }
    if( pending_population <= 0 ){
        GWSExecutionEnvironment::globalInstance()->run();
    }




    // LISTEN TO EXTERNAL SIMULATIONS
    QJsonObject json_external_listeners = json_configuration.value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        GWSCommunicationEnvironment::globalInstance()->connectExternalEnvironment( json_external_listeners.value( key ).toString() );
        qDebug() << QString("Creating external listener %1 to %2").arg( key ).arg( json_external_listeners.value( key ).toString() );
     }




GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
    QDateTime current_time = QDateTime::currentDateTime();
    qint64 secondsDiff = start.secsTo( current_time );
    qDebug() << "Elapsed time" << secondsDiff;
});

app->exec();

}
