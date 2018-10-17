#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QtMath>

#include "../../agent/Agent.h"

// Agents
#include "ContainerAgent.h"
#include "HumanAgent.h"
#include "TruckAgent.h"


// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/check/CheckIfAtPositionBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteBehaviour.h"
#include "../../behaviour/waste4think/FindClosestBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/graph/Graph.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/routing/TSPRouting.h"
//#include "../../util/grid/Grid.h"

#include <time.h>
#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // Init used environments
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();

    // Agents
    GWSObjectFactory::globalInstance()->registerType( ContainerAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TruckAgent::staticMetaObject );

    // Skills
    GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteSkill::staticMetaObject );

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransferAgentPropertyBehaviour::staticMetaObject );
    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


QJsonDocument json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", "
                                                        "\"@family\": [ \"GWSAgent\", \"Citizen\" ], "
                                                        "\"home_x\": -2, \"home_y\": 43, \"waste\": 100 , "
                                                        "\"@behaviours\": [ { \"duration\": 1, \"maxspeed\": 4, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"start\": true, \"nexts_if_arrived\" : [\"FIND\"] }, "
                                                                    "{ \"duration\": 1, \"\@id\": \"FIND\", \"closest_agent_type\": \"ContainerAgent\", \"transport_network_type\": \"Road\", \"store_closest_id_as\": \"closest_container_id\", \"store_closest_distance_as\": \"closest_container_distance\", \"@id\": \"FIND\", \"nexts\": [ \"TRANSFER\" ] }, "
                                                                    "{ \"duration\": 1, \"@id\": \"TRANSFER\", \"property_to_transfer\": \"waste\", \"receiving_agent_id\": \"<closest_container_id>\" } "
                                                        "] } ")
                                                        .toLatin1()
                                                        );


QSharedPointer<GWSAgent> element = GWSObjectFactory::globalInstance()->fromJSON( json.object() ).dynamicCast<GWSAgent>();
qDebug() << element->serialize();



}
