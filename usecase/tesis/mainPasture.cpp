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


    /* -------------
     * Grass Agents
     * -------------*/

    // Populate a zone of size A x B with GrassAgents

    for( int i = -5 ; i < 5 ; i += 1 ){
       for( int j = -5 ; j < 5 ; j += 1 ){

           QJsonDocument jsonPasture = QJsonDocument::fromJson( QString("{ \"@type\" : \"PastureAgent\" , \
                                                       \"energy\" : 7, \
                                                       \"geo\" : {  \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , \
                                                       \"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/628/628296.svg\" , \"color\" : \"green\" } , \
                                                       \"@behaviours\" : [ \
                                                                            { \"@type\" : \"GWSBehaviour\" , \"@id\" :  \"BH\" , \"duration\" : 60000 } , \
                                                                            { \"@type\" : \"IncrementPropertyBehaviour\" , \"start\" : true ,  \"property\" : \"energy\" , \"increment\" : 1. , \"max\" : 50. , \"min\" : 0 , \"duration\" : 1000 , \"@next\" : [\"BH\"] } ] \
                                                       } ")
                                                       .arg( i )
                                                       .arg( j )
                                                       .toLatin1()
                                                       );

           PastureAgent* pasture = dynamic_cast<PastureAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonPasture.object() ) );
           GWSExecutionEnvironment::globalInstance()->registerAgent( pasture );
    }
    }




    // The PastureAgent Simulation needs information from the SheepAgent Simulation
    GWSExternalListener* ext = new GWSExternalListener("SheepSim");

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();

}
