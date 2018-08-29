#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QSharedPointer>


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

    // Skills
    GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveSkill::staticMetaObject );

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckAliveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( EatBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( BreedBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SelectDestinationBehaviour::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    /* ----------
     * WolfAgents
       ----------*/

    for( int i = 0 ; i < 50 ; i++ ){


        /* Nymeria1 */
        QJsonDocument jsonPredator = QJsonDocument::fromJson( QString("{ \"@type\" : \"PredatorAgent\" , "
                                                                     "\"energy\" : 50.0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-0.1, -0.1],[-0.1, 0.1],[0.1, 0.1],[0.1, -0.1],[-0.1, -0.1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveSkill\" , \"maxspeed\" : 80000 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/235/235427.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                        "{ \"@type\" : \"SelectDestinationBehaviour\" , \"@id\" : \"BH1\" , \"duration\" : 1000 } , "
                                                                                        "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH3\" , \"@sub_behaviours\" : ["
                                                                                                                    "{ \"@type\" : \"EatBehaviour\", \"prey\" : \"SheepAgent\" , \"duration\" : 1000 }, "
                                                                                                                    "{ \"@type\" : \"BreedBehaviour\", \"duration\" : 1000 } "
                                                                                                     "] } , "
                                                                                        "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH2\" , \"@next\" : [\"BH3\"] , \"@sub_behaviours\" : ["
                                                                                                                                                           "{ \"@type\" : \"MoveBehaviour\", \"duration\" : 1000 } , "
                                                                                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : -1., \"duration\" : 1000 } "
                                                                                                                                            "] } ,"
                                                                                        "{ \"@type\" : \"CheckAliveBehaviour\" , \"duration\" : 1000 , \"start\" : true , \"@next\" : [\"BH1\", \"BH2\"] } "
                                                                   " ] } ")
                                                       .arg( qrand() % 10 - 5 )
                                                       .arg( qrand() % 10 - 5 )
                                                       .toLatin1()
                                                        );




        QSharedPointer<GWSAgent> predator = GWSObjectFactory::globalInstance()->fromJSON( jsonPredator.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( predator );

    }


    GWSExternalListener* ext = new GWSExternalListener("SheepSim");
    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();

}
