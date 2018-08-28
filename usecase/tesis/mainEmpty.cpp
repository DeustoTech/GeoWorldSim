#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>


#include "TesisBehaviour.h"
#include "../../agent/Agent.h"
#include "../../behaviour/Behaviour.h"

// Agents
//#include "TerrainAgent.h"
#include "PastureAgent.h"
#include "SheepAgent.h"
#include "PredatorAgent.h"

#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"

#include "../../behaviour/property/IncrementPropertyBehaviour.h"
#include "../../behaviour/eat/EatBehaviour.h"
#include "../../behaviour/breed/BreedBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/SelectDestinationBehaviour.h"
#include "../../behaviour/alive/CheckAliveBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
//#include "../../environment/grid_environment/GridEnvironment.h"

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
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
    //GWSGridEnvironment::globalInstance();


    // Init Object Factory
    //GWSObjectFactory::globalInstance()->registerType( TerrainAgent::staticMetaObject );

    // Agents
    GWSObjectFactory::globalInstance()->registerType( PastureAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SheepAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PredatorAgent::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    GWSExternalListener* ext1 = new GWSExternalListener("PastureSim");
    GWSExternalListener* ext2 = new GWSExternalListener("SheepSim");
    GWSExternalListener* ext3 = new GWSExternalListener("WolfSim");

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
