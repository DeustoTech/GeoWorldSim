#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "TerrainAgent.h"
#include "TesisBehaviour.h"
#include "../../agent/Agent.h"
#include "../../behaviour/Behaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/grid/Grid.h"

int main(int argc, char* argv[])
{
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // Init used environments
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();

    // Init Object Factory

    QJsonDocument json = QJsonDocument::fromJson( "{ \"@type\" : \"GWSAgent\" , \"@id\" : \"mygrid\" , \"geo\" : { \"@type\" : \"GWSPoint\" } , \"grid\" : { \"@type\": \"GWSGrid\" , \"values\" : [ [1,2] , [3,4]] ,  \"max_value\": 100 , \"min_value\" : 0 } }" );
    GWSAgent* agent1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json.object() ) );

    GWSBehaviour* b1 = new GWSBehaviour( agent1 );
    TesisBehaviour* b11 = new TesisBehaviour( agent1 );
    TesisBehaviour* b12 = new TesisBehaviour( agent1 );
    agent1->setStartBehaviour( b1 );
    b1->addSubbehaviour( b11 );
    b1->addSubbehaviour( b12 );

    TesisBehaviour* b2 = new TesisBehaviour( agent1 );
    b1->setNextBehaviour( b2 );

    // Register in environments
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent1 );
    QList<GWSAgent*> l = GWSAgentEnvironment::globalInstance()->getByClass<GWSAgent>( GWSAgent::staticMetaObject.className() );

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();
}
