#include "app/App.h"
#include "object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "agent/Agent.h"
#include "skill/Skill.h"
#include "behaviour/Behaviour.h"

#include "environment/Environment.h"
#include "environment/agent_environment/AgentEnvironment.h"
#include "environment/execution_environment/ExecutionEnvironment.h"

int main(int argc, char* argv[])
{
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );


    QJsonObject obj = QJsonDocument::fromJson("{ \
                                              \"@context\": \"http://schema.org\", \
                                              \"@type\": \"GWSAgent\", \
                                              \"name\": \"George Bush\", \
                                              \"disambiguatingDescription\": \"41st President of the United States\", \
                                              \"children\": { \
                                                \"@type\": \"GWSAgent\", \
                                                \"name\": \"George W. Bush\", \
                                                \"disambiguatingDescription\": \"43rd President of the United States\" \
                                              } \
                                            }").object();


    GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->create( obj ) );
    GWSEnvironment::globalInstance()->registerAgent( agent );
    GWSExecutionEnvironment::globalInstance()->runAgent( agent );

    agent->connect( agent , &GWSAgent::agentBehavedSignal , [agent](){
        qDebug() << agent->serialize();
    });

    app->exec();
}
