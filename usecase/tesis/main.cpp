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
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/SelectDestinationBehaviour.h"
#include "../../behaviour/alive/CheckAliveBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
//#include "../../environment/grid_environment/GridEnvironment.h"

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
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
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SelectDestinationBehaviour::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* -------------
     * Grass Agents
     * -------------*/

    // Populate a zone of size A x B with GrassAgents

    for( int i = -50 ; i < 50 ; i += 5 ){
       for( int j = -50 ; j < 50 ; j += 5 ){

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



    /* ----------
     * SheepAgents
       ----------*/

    /* Dolly1 */
    for( int i = 0 ; i < 500 ; i++ ){

        QJsonDocument jsonSheep = QJsonDocument::fromJson( QString("{ \"@type\" : \"SheepAgent\" , "
                                                                     "\"energy\" : 50.0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-0.1, -0.1],[-0.1, 0.1],[0.1, 0.1],[0.1, -0.1],[-0.1, -0.1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveSkill\" , \"maxspeed\" : 80000 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" , \"color\" : \"blue\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                        "{ \"@type\" : \"SelectDestinationBehaviour\" , \"@id\" : \"BH1\" , \"duration\" : 1000 } , "
                                                                                        "{ \"@type\" : \"EatBehaviour\" , \"@id\" : \"BH3\" , \"prey\" : \"PastureAgent\" , \"increment_time\" : 1000 } , "
                                                                                        "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH2\" , \"@next\" : [\"BH3\"] , \"@sub_behaviours\" : ["
                                                                                                                                                           "{ \"@type\" : \"MoveBehaviour\", \"duration\" : 1000 } , "
                                                                                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : -0.1, \"duration\" : 1000 } "
                                                                                                                                            "] } ,"
                                                                                        "{ \"@type\" : \"CheckAliveBehaviour\" , \"duration\" : 1000 , \"start\" : true , \"@next\" : [\"BH1\", \"BH2\"] } "
                                                                   " ] } ")
                                                       .arg( qrand() % 100 - 50 )
                                                       .arg( qrand() % 100 - 50 )
                                                       .toLatin1()
                                                        );


        GWSAgent* sheep = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonSheep.object() ) );

        GWSExecutionEnvironment::globalInstance()->registerAgent( sheep );

    }


    /* ----------
     * WolfAgents
       ----------*/

    for( int i = 0 ; i < 500 ; i++ ){

        /* Nymeria1 */
        QJsonDocument jsonPredator = QJsonDocument::fromJson( QString("{ \"@type\" : \"PredatorAgent\" , "
                                                                     "\"energy\" : 50.0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-0.1, -0.1],[-0.1, 0.1],[0.1, 0.1],[0.1, -0.1],[-0.1, -0.1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveSkill\" , \"maxspeed\" : 80000 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/235/235427.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                        "{ \"@type\" : \"SelectDestinationBehaviour\" , \"@id\" : \"BH1\" , \"duration\" : 1000 } , "
                                                                                        "{ \"@type\" : \"EatBehaviour\" , \"@id\" : \"BH3\" , \"prey\" : \"SheepAgent\" , \"increment_time\" : 1000 } , "
                                                                                        "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH2\" , \"@next\" : [\"BH3\"] , \"@sub_behaviours\" : ["
                                                                                                                                                           "{ \"@type\" : \"MoveBehaviour\", \"duration\" : 1000 } , "
                                                                                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : -0.1, \"duration\" : 1000 } "
                                                                                                                                            "] } ,"
                                                                                        "{ \"@type\" : \"CheckAliveBehaviour\" , \"duration\" : 1000 , \"start\" : true , \"@next\" : [\"BH1\", \"BH2\"] } "
                                                                   " ] } ")
                                                       .arg( qrand() % 100 - 50 )
                                                       .arg( qrand() % 100 - 50 )
                                                       .toLatin1()
                                                        );




        GWSAgent* predator = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonPredator.object() ) );


        GWSExecutionEnvironment::globalInstance()->registerAgent( predator );

    }

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
