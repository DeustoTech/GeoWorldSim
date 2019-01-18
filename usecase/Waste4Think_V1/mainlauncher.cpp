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
#include "ContainerAgent.h"
#include "HumanAgent.h"
#include "TruckAgent.h"
#include "RecyclingPlantAgent.h"


// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/MoveThroughRouteInVehicleSkill.h"
#include "../../skill/pollute/PolluteSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/waste4think/DelayBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/waste4think/FindClosestBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/waste4think/FollowTSPRouteBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteInVehicleBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/waste4think/GatherAgentPropertyBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/waste4think/CheckPropertyValueBehaviour.h"
#include "../../behaviour/waste4think/GenerateRandomValueBehaviour.h"
#include "../../behaviour/waste4think/ChooseRandomValueFromSetBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/electricTravelling/DriveBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( ContainerAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TruckAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( RecyclingPlantAgent::staticMetaObject);

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteZamudioModelBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( DelayBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransferAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CalculateTSPRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FollowTSPRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteInVehicleBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( GatherAgentPropertyBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CopyPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckPropertyValueBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( GenerateRandomValueBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ChooseRandomValueFromSetBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject ) ;
    GWSObjectFactory::globalInstance()->registerType( DriveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ListenToMessagesBehaviour::staticMetaObject );


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

        if ( !population.value("template").isNull() && !population.value("datasource_urls").isNull() ){
            pending_population++;

            QJsonArray datasources = population.value("datasource_urls").toArray();

            for ( int i = 0; i <  datasources.size(); ++i){

                QString ds_url = datasources.at(i).toString();

                GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value("template").toObject() ,  ds_url );
                ds->connect( ds , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [ds,&pending_population](){
                    pending_population--;
                    ds->deleteLater();
                    if( pending_population <= 0 ){
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
    if( pending_population <= 0 ){
        GWSExecutionEnvironment::globalInstance()->run();
    }




    // LISTEN TO EXTERNAL SIMULATIONS
    /*QJsonObject json_external_listeners = json_configuration.value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        if ( !json_external_listeners[ key ].isNull() ){
            new GWSExternalListener( json_external_listeners[ key ].toString() );
        }
        qDebug() << QString("Creating external listener %1").arg( key );
     }*/




GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
    QDateTime current_time = QDateTime::currentDateTime();
    qint64 secondsDiff = start.secsTo( current_time );
    qDebug() << "Elapsed time" << secondsDiff;
});

app->exec();

}






