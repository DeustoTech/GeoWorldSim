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

    for( int i = 0 ; i < 0 ; i ++ ){
       for( int j = 0 ; j < 10 ; j++ ){

           QJsonDocument jsonPasture = QJsonDocument::fromJson( QString("{ \"@type\" : \"PastureAgent\" , \
                                                       \"energy\" : 7, \
                                                       \"geo\" : {  \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , \
                                                       \"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/628/628296.svg\" } , \
                                                       \"@behaviours\" : [ { \"@type\" : \"IncrementPropertyBehaviour\" , \"@start\" : true ,  \"property\" : \"energy\" , \"increment\" : 1. , \"max\" : 50. , \"@forward_time\" : 1000 } ] \
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
    for( int i = 0 ; i < 10 ; i++ ){

        QJsonDocument jsonSheep = QJsonDocument::fromJson( QString("{ \"@type\" : \"SheepAgent\" , "
                                                                     "\"energy\" : 50.0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [0, 0] } } , "
                                                                                     "{ \"@type\" : \"MoveSkill\" , \"maxspeed\" : 10000 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                        "{ \"@type\" : \"EatBehaviour\" , \"@id\" : \"BH2\" \"prey\" : \"PastureAgent\", \"increment_time\" : 1000 } , "
                                                                                        "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH1\" , \"@sub_behaviours\" : ["
                                                                                                                                                   "{ \"@type\" : \"SelectDestinationBehaviour\" , \"@forward_time\" : 0 } , "
                                                                                                                                                   "{ \"@type\" : \"GWSBehaviour\" , \"@sub_behaviours\" : ["
                                                                                                                                                                                                           "{ \"@type\" : \"MoveBehaviour\", \"@forward_time\" : 1000 , \"@next\" : \"BH2\" } , "
                                                                                                                                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : -5. , \"@forward_time\" : 1000 } "
                                                                                                                                                   "] } "
                                                                                                                                                "] } ,"
                                                                                        "{ \"@type\" : \"CheckAliveBehaviour\" , \"@forward_time\" : 0 , \"@start\" : true , \"@next\" : \"BH1\" } "
                                                                   " ] } ")
                                                       .arg( qrand() % 5 )
                                                       .arg( qrand() % 5 )
                                                       .toLatin1()
                                                       );                                                                       

        GWSAgent* sheep = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonSheep.object() ) );

        GWSExecutionEnvironment::globalInstance()->registerAgent( sheep );

    }


    /* ----------
     * WolfAgents
       ----------*/

    for( int i = 0 ; i < 5 ; i++ ){

        /* Nymeria1 */
        QJsonDocument jsonPredator =  QJsonDocument::fromJson( QString("{ \"@type\" : \"PredatorAgent\" , "
                                                                       "\"energy\" : 50.0 , "
                                                                       "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [0, 0] } } , "
                                                                                       "{ \"@type\" : \"MoveSkill\" , \"maxspeed\" : 10000 } ],"
                                                                        "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                                        "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/235/235427.svg\" } , "
                                                                       "\"@behaviours\" : [ { \"@type\" : \"CheckAliveBehaviour\" , \"@forward_time\" : 0 } , "
                                                                                           "{ \"@type\" : \"GWSBehaviour\" , \"@sub_behaviours\" : ["
                                                                                                                                                  "{ \"@type\" : \"MoveBehaviour\", \"@forward_time\" : 1000 } , "
                                                                                                                                                  "{ \"@type\" : \"SelectDestinationBehaviour\" , \"@forward_time\" : 0 } ,"
                                                                                                                                                  "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : -5. , \"@forward_time\" : 1000 } , "
                                                                                                                                                  "{ \"@type\" : \"EatBehaviour\" , \"prey\" : \"SheepAgent\", \"increment_time\" : 1000 }  "
                                                                                                                                                  "]  } ] } ")
                                                                       .arg( qrand() % 5 )
                                                                       .arg( qrand() % 5 )
                                                                       .toLatin1()
                                                                       );


                                                                         // "\"@behaviours\" : [ { \"@type\" : \"EatBehaviour\" , \"prey\" : \"SheepAgent\", \"increment_time\" : 1000 } ] "

        GWSAgent* predator = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonPredator.object() ) );


        GWSExecutionEnvironment::globalInstance()->registerAgent( predator );

    }

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
