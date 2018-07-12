#include "../../app/App.h"
#include "./../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "./../agent/Agent.h"
#include "./../skill/Skill.h"
#include "./../behaviour/Behaviour.h"

#include "./../environment/Environment.h"
#include "./../environment/agent_environment/AgentEnvironment.h"
#include "./../environment/execution_environment/ExecutionEnvironment.h"

int main(int argc, char* argv[])
{
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    GWSEnvironment::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();

    QJsonObject obj = QJsonDocument::fromJson("{ \
                                              \"@context\": \"http://schema.org\", \
                                              \"@id\": \"Person123412\", \
                                              \"@type\": \"GWSAgent\", \
                                              \"@inheritance\" : [\"GWSObject\",\"GWSAgent\",\"Person\"], \
                                              \"@name\": \"George Bush\" \
                                            }").object();


    GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->create( obj ) );
    GWSEnvironment::globalInstance()->registerAgent( agent );
    qDebug() << GWSAgentEnvironment::globalInstance()->getByClassAndId( "GWSAgent" , "Person123412" )->serialize();

    app->exec();
}
