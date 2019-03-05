#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtMath>

#include "../../agent/Agent.h"

// Agents
#include "HumanAgent.h"


// Skills
#include "../../skill/Skill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/property/CompareAgentPropertyBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/property/SetAgentPropertyBehaviour.h"
#include "../../behaviour/accessibility/DetermineAccessibilityBehaviour.h"
#include "../../behaviour/accessibility/FindDirectClosestBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"
#include "../../behaviour/waste4think/ChooseRandomValueFromSetBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindDirectClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindRoutingClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CompareAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ListenToMessagesBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( DetermineAccessibilityBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ChooseRandomValueFromSetBehaviour::staticMetaObject );


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

        if ( !population.value( "template" ).isNull() && !population.value( "datasources" ).isNull() ){

            QJsonArray datasources = population.value( "datasources" ).toArray();

            for ( int i = 0; i <  datasources.size() ; ++i ){

                QJsonObject datasource = datasources.at( i ).toObject();
                QString datasource_id = datasource.value("id").toString();
                QJsonArray entities_type = datasource.value("entities").toArray();
                if( datasource_id.isEmpty() || entities_type.isEmpty() ){
                    qWarning() << "Asked to download from scenario without ID or entities type";
                }

                for ( int j = 0; j < entities_type.size() ; ++j ){

                    QString entity = entities_type.at( j ).toString();

                    GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value( "template" ).toObject() , datasource_id,  entity );
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




GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
    QDateTime current_time = QDateTime::currentDateTime();
    qint64 secondsDiff = start.secsTo( current_time );
    qDebug() << "Elapsed time" << secondsDiff;
});

app->exec();

}






