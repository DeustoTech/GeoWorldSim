#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>

#include "TerrainAgent.h"
#include "TesisBehaviour.h"
#include "../../agent/Agent.h"
#include "../../behaviour/Behaviour.h"

// Agents
#include "TerrainAgent.h"
#include "SheepAgent.h"
#include "PredatorAgent.h"

#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/grid/Grid.h"

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
    GWSGridEnvironment::globalInstance();


    // Init Object Factory
    GWSObjectFactory::globalInstance()->registerType( TerrainAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SheepAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PredatorAgent::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    /* -------------
     * Terrain Agent
     * -------------*/

    QJsonDocument jsonTerrain = QJsonDocument::fromJson( "{ \"@type\" : \"TerrainAgent\" , "
                                                   "\"@id\" : \"ThePlayground\" ,"
                                                   "\"grid_x_size\" : 10000, "
                                                   "\"grid_y_size\" : 10000  "
                                                   "}"
                                                   );
    TerrainAgent* terrain = dynamic_cast<TerrainAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonTerrain.object() ) );
    GWSEnvironment::globalInstance()->registerAgent( terrain );
    GWSExecutionEnvironment::globalInstance()->registerAgent( terrain );

    qInfo()<< "I am a GWSAgent of" << terrain->property("@type").toString() << "type.";


    /* ----------
     * SheepAgents
       ----------*/

    /* Dolly1 */
    for( int i = 0 ; i < 50 ; i++ ){

        QJsonDocument json1 = QJsonDocument::fromJson( QString("{ \"@type\" : \"SheepAgent\" , "
                                                      "\"energy\" : 100.0 , "
                                                       "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                       "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" }  }" )
                                                       .arg( qrand() % 100 )
                                                       .arg( qrand() % 100 )
                                                       .toLatin1()
                                                       );

        GWSAgent* sheep = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json1.object() ) );
        qInfo()<< "I am a GWSAgent of" << sheep->getProperty( GWSAgent::GWS_TYPE_PROP ) << "type.";

        /* Notify the grid of the presence of a sheep at current position*/
        terrain->addGridCellValue(sheep->getCentroid().getX(), sheep->getCentroid().getY(), sheep);

        GWSEnvironment::globalInstance()->registerAgent( sheep );
        GWSExecutionEnvironment::globalInstance()->registerAgent( sheep );

    }


    /* ----------
     * WolfAgents
       ----------*/

    for( int i = 0 ; i < 50 ; i++ ){

        /* Nymeria1 */
        QJsonDocument json4 = QJsonDocument::fromJson( QString("{ \"@type\" : \"PredatorAgent\" , "
                                                      "\"energy\" : 200.0 , "
                                                      "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [%1 , %2 , 0]} , "
                                                       "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/235/235427.svg\" } "
                                                      "}")
                                                       .arg( qrand() % 100 )
                                                       .arg( qrand() % 100 )
                                                       .toLatin1()
                                                       );

        GWSAgent* predator = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json4.object() ) );

        qInfo()<< "I am a GWSAgent of" << predator->getProperty( GWSAgent::GWS_TYPE_PROP ) << "type.";

        /* Notify the grid of the presence of a wolf at current position */
        terrain->addGridCellValue( predator->getCentroid().getX(), predator->getCentroid().getY(), predator );

        GWSEnvironment::globalInstance()->registerAgent( predator );
        GWSExecutionEnvironment::globalInstance()->registerAgent( predator );

    }

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
