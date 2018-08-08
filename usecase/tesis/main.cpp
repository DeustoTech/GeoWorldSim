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

#include "../../behaviour/property/IncrementPropertyBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( PastureAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SheepAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PredatorAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* -------------
     * Grass Agents
     * -------------*/

    // Populate a zone of size A x B with GrassAgents

    for( int i = 0 ; i < 5 ; i ++ ){
       for( int j = 0 ; j < 5 ; j++ ){

           QJsonDocument jsonPasture = QJsonDocument::fromJson( QString("{ \"@type\" : \"PastureAgent\" , \
                                                       \"energy\" : 7, \
                                                       \"geo\" : {  \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , \
                                                       \"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/628/628296.svg\" } , \
                                                       \"@behaviours\" : [ { \"@type\" : \"IncrementPropertyBehaviour\" , \"property\" : \"energy\" , \"increment\" : 1. , \"max\" : 15. , \"increment_time\" : 1000 } ] \
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
    for( int i = 0 ; i < 15 ; i++ ){

        QJsonDocument json1 = QJsonDocument::fromJson( QString("{ \"@type\" : \"SheepAgent\" , "
                                                      "\"energy\" : 50.0 , "
                                                      "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [0, 0] } } ] ,"
                                                       "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                       "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" }  }" )
                                                       .arg( qrand() % 5 )
                                                       .arg( qrand() % 5 )
                                                       .toLatin1()
                                                       );

        GWSAgent* sheep = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json1.object() ) );

        GWSExecutionEnvironment::globalInstance()->registerAgent( sheep );

    }


    /* ----------
     * WolfAgents
       ----------*/

    for( int i = 0 ; i < 15 ; i++ ){

        /* Nymeria1 */
        QJsonDocument json4 = QJsonDocument::fromJson( QString("{ \"@type\" : \"PredatorAgent\" , "
                                                      "\"energy\" : 50.0 , "
                                                      "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"GWSAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [0, 0] } } ] ,"
                                                      "\"geo\" : {  \"@type\" : \"GWSGeometry\", \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                       "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/235/235427.svg\" } "
                                                      "}")
                                                       .arg( qrand() % 5 )
                                                       .arg( qrand() % 5 )
                                                       .toLatin1()
                                                       );

        GWSAgent* predator = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json4.object() ) );


        GWSExecutionEnvironment::globalInstance()->registerAgent( predator );

    }

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
