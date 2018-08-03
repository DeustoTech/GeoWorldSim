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


    /* -------------
     * Terrain Agent
     * -------------*/

    QJsonDocument jsonTerrain = QJsonDocument::fromJson( "{ \"@type\" : \"TerrainAgent\" , "
                                                   "\"@id\" : \"ThePlayground\" ,"
                                                   "\"grid_x_size\" : 100, "
                                                   "\"grid_y_size\" : 100  "
                                                   "}"
                                                   );
    TerrainAgent* terrain = dynamic_cast<TerrainAgent*>( GWSObjectFactory::globalInstance()->fromJSON( jsonTerrain.object() ) );

    qInfo()<< "I am a GWSAgent of" << terrain->property("@type").toString() << "type.";
    qInfo()<< "And my size is" << terrain->property("grid_x_size").toInt() << "x"<< terrain->property("grid_y_size").toInt();



    /* ----------
     * SheepAgents
       ----------*/

    /* Dolly1 */
    QJsonDocument json1 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                   "\"@id\" : \"Dolly1\" , "
                                                  "\"energy\" : 10.0 , "
                                                   "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [50 , 50 , 0]} , "
                                                   "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" }  }" );
                                                    //"\"geo\" : { \"type\" : \"Polygon\" , \"coordinates\" : [[[0,0],[0,1],[1,1],[1,0],[0,0]]] }"

    GWSAgent* sheep1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json1.object() ) );

    qInfo()<< "I am a GWSAgent of" << sheep1->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a sheep at current position*/
    terrain->addGridCellValue(sheep1->getCentroid().getX(), sheep1->getCentroid().getY(), sheep1);


    /* Dolly2 */
    QJsonDocument json2 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly2\" , "
                                                  "\"energy\" : 5.0 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [51 , 51 , 0]} , "
                                                  "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" } "
                                                  "}" );

    GWSAgent* sheep2 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json2.object() ) );

    qInfo()<< "I am a GWSAgent of" << sheep2->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a sheep at current position */
    terrain->addGridCellValue(sheep2->getCentroid().getX(), sheep2->getCentroid().getY(), sheep2);


    /* Dolly3 */
    QJsonDocument json3 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly3\" , "
                                                  "\"energy\" : 20.0 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [49 , 49 , 0]} , "
                                                  "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/801/801373.svg\" } "
                                                  "}" );

    GWSAgent* sheep3 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json3.object() ) );

    qInfo()<< "I am a GWSAgent of" << sheep3->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a sheep at current position */
    terrain->addGridCellValue(sheep3->getCentroid().getX(), sheep3->getCentroid().getY(), sheep3);


    /* ----------
     * WolfAgents
       ----------*/

    /* Nymeria1 */
    QJsonDocument json4 = QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria1\" , "
                                                  "\"energy\" : 20.0 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [50 , 49 , 0]}"
                                                  "}" );

    GWSAgent* predator1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json4.object() ) );

    qInfo()<< "I am a GWSAgent of" << predator1->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a wolf at current position */
    terrain->addGridCellValue(predator1->getCentroid().getX(), predator1->getCentroid().getY(), predator1);


    /* Nymeria2 */
    QJsonDocument json5 = QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria2\" , "
                                                  "\"energy\" : 15.0 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [49 , 51 , 0]}"
                                                  "}" );

    GWSAgent* predator2 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json5.object() ) );

    qInfo()<< "I am a GWSAgent of" << predator2->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a wolf at current position */
    terrain->addGridCellValue(predator2->getCentroid().getX(), predator2->getCentroid().getY(), predator2);


    /* Nymeria3 */
    QJsonDocument json6= QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria3\" , "
                                                  "\"energy\" : 10.0 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [51 , 45 , 0]}"
                                                  "}" );

    GWSAgent* predator3 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json6.object() ) );

    qInfo()<< "I am a GWSAgent of" << predator3->property("@type").toString() << "type.";

    /* Notify the grid of the presence of a wolf at current position:*/
    terrain->addGridCellValue(predator3->getCentroid().getX(), predator3->getCentroid().getY(), predator3);



    /* Register Agents in environments */
    //GWSGridEnvironment::globalInstance()->registerAgent( terrain );
    GWSExecutionEnvironment::globalInstance()->registerAgent( terrain );
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep2 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep3 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator2 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator3 );

    GWSAgentEnvironment::globalInstance()->registerAgent( terrain );
    GWSAgentEnvironment::globalInstance()->registerAgent( sheep1 );
    GWSAgentEnvironment::globalInstance()->registerAgent( sheep2 );
    GWSAgentEnvironment::globalInstance()->registerAgent( sheep3 );
    GWSAgentEnvironment::globalInstance()->registerAgent( predator1 );
    GWSAgentEnvironment::globalInstance()->registerAgent( predator2 );
    GWSAgentEnvironment::globalInstance()->registerAgent( predator3 );

    QList<GWSAgent*> l = GWSAgentEnvironment::globalInstance()->getByClass<GWSAgent>( GWSAgent::staticMetaObject.className() );

    GWSExecutionEnvironment::globalInstance()->run();


    app->exec();

}
