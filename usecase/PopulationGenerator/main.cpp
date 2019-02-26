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

// Behaviours:
#include "../../behaviour/population/GeneratePopulationBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/waste4think/WaitUntilTimeBehaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/population/CreateChildBehaviour.h"

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

        if ( !population.value("template").isNull() && !population.value("datasource_urls").isNull() ){

            QJsonArray datasources = population.value("datasource_urls").toArray();

            for ( int i = 0; i <  datasources.size(); ++i){

                QString ds_url = datasources.at(i).toString();

                GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value("template").toObject() ,  ds_url );
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


    app->exec();
}
