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
#include "../../behaviour/geometry/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/time/WaitUntilTimeBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/accessibility/FindRoutingClosestBehaviour.h"
#include "../../behaviour/transaction/TransactionBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteInVehicleBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/property/GatherAgentPropertyBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/property/CheckPropertyValueBehaviour.h"
#include "../../behaviour/random/ChooseRandomValueFromSetBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"
#include "../../behaviour/electricTravelling/DriveBehaviour.h"
#include "../../behaviour/information/ListenToMessagesBehaviour.h"
#include "../../behaviour/random/GenerateRandomValueBehaviour.h"
#include "../../behaviour/property/SetAgentPropertyBehaviour.h"
#include "../../behaviour/property/IncrementPropertyBehaviour.h"
#include "../../behaviour/property/MathAgentPropertyBehaviour.h"

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

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // INIT OBJECT FACTORY
    GWSObjectFactory::globalInstance()->registerType( GWSAgent::staticMetaObject );


    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();
    GWSCommunicationEnvironment::globalInstance();

    // AVAILABLE BEHAVIOURS
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteZamudioModelBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( WaitUntilTimeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindRoutingClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransactionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CalculateTSPRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetNextTSPDestinationBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteInVehicleBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( GatherAgentPropertyBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CopyPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckPropertyValueBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( ChooseRandomValueFromSetBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ListenToMessagesBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateRandomValueBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MathAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( WaitUntilTimeBehaviour::staticMetaObject );


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






