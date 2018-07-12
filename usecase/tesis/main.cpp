#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "../../agent/Agent.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"

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

    // Create agent from JSON
    QJsonObject obj = QJsonDocument::fromJson("{ \
                                              \"@context\": \"http://schema.org\", \
                                              \"@id\": \"Person1\", \
                                              \"@type\": \"GWSAgent\", \
                                              \"@inheritance\" : [\"GWSObject\",\"GWSAgent\",\"Person\"], \
                                              \"name\": \"George Bush\" \
                                            }").object();
    GWSAgent* agent1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->create( obj ) );

    // Create agent normal style
    GWSAgent* agent2 = new GWSAgent();
    agent2->setProperty( GWSAgent::GWS_ID_PROP , "Person2");

    // Register in environments
    GWSAgentEnvironment::globalInstance()->registerAgent( agent1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent1 );

    // Get from agent environment
    qDebug() << GWSAgentEnvironment::globalInstance()->getByClassAndId( "GWSAgent" , "Person1" )->serialize();

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();
}
