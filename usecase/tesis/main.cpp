#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "TerrainAgent.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"

int main(int argc, char* argv[])
{
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // Init used environments
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSGridEnvironment::globalInstance();


    // Create agent normal style
    TerrainAgent* agent2 = new TerrainAgent();
    agent2->setProperty( "message" , "Hello" );
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent2 );

    // Register in environments

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();
}
