#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>


// Include PopulationGenerator files:
//#include "header.h"

#include "PopulationGeneratorAgent.h"

// Include GWS files:
#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../object/Object.h"
#include "../../behaviour/Behaviour.h"
#include "../../agent/Agent.h"

// Skills:
#include "../../skill/move/MoveSkill.h"

// Behaviours:
#include "../../behaviour/population/GeneratePopulationBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/time/WaitUntilTimeBehaviour.h"
#include "../../behaviour/geometry/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/population/CreateChildBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"

// Utils:
#include "../../util/routing/Routing.h"
#include "../../util/routing/GraphEdgeVisitor.h"
#include "../../util/routing/TSPRouting.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Quadtree.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/AgentGeneratorDatasource.h"

// Environments:
#include "../../environment/Environment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/network_environment/NetworkEdge.h"
#include "../../environment/grid_environment/GridEnvironment.h"



/**************************************************************************************
  M A I N
***************************************************************************************/
int main( int argc, char* argv[] )
{

    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // INIT OBJECT FACTORY
    GWSObjectFactory::globalInstance()->registerType( PopulationGeneratorAgent::staticMetaObject );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GeneratePopulationBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( WaitUntilTimeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CreateChildBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );


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
                QString datasource_id = datasource.value("id").toString();
                QJsonArray entities_type = datasource.value("entities").toArray();
                if( datasource_id.isEmpty() || entities_type.isEmpty() ){
                    qWarning() << "Asked to download from scenario without ID or entities type";
                }

                for ( int j = 0; j < entities_type.size() ; ++j ){

                    QString entity = entities_type.at( j ).toString();

                    GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value( "template" ).toObject() , datasource_id,  entity );
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
