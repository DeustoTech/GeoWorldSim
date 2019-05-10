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
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/property/CompareAgentPropertyBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/property/SetAgentPropertyBehaviour.h"
#include "../../behaviour/accessibility/FindDirectClosestBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"
#include "../../behaviour/property/AddGWSGroupPropertyBehaviour.h"
#include "../../behaviour/transaction/TransactionBehaviour.h"

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
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE AGENTS
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( FindDirectClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindRoutingClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CompareAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( AddGWSGroupPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransactionBehaviour::staticMetaObject );


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
                QString entity_filter = datasource.value("entity_filter").toString();
                if( scenario_id.isEmpty() || entity_type.isEmpty() ){
                    qWarning() << "Asked to download from scenario without ID or entity_type";
                }

                GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value( "template" ).toObject() , scenario_id,  entity_type , entity_filter ,  limit > 0 ? limit : 999999999999999 );
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






