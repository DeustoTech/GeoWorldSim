#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "TerrainAgent.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

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

    GWSObjectFactory::globalInstance()->registerType( GWSGrid::staticMetaObject );

    QJsonDocument json = QJsonDocument::fromJson( "{ \"@type\" : \"GWSAgent\" , \"@id\" : \"mygrid\" , \"geo\" : { \"@type\": \"GWSGrid\" , \"latitude\": 40.75 , \"longitude\" : 73.98 } }" );
    GWSAgent* agent1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->create( json.object() ) );
    agent1->setProperty( "message" , "Hello" );

    // Register in environments
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent1 );

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();
}
