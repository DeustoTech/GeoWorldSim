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





    // Init Object Factory
    GWSObjectFactory::globalInstance()->registerType( SheepAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PredatorAgent::staticMetaObject );


    /* ----------
    /* SheepAgents
       ----------*/

    /* Dolly1 */
    QJsonDocument json1 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                   "\"@id\" : \"Dolly1\" , "
                                                  "\"energy\" : 10 , "
                                                  "\"geo\" : { \"type\" : \"Polygon\" , \"coordinates\" : [[[0,0],[0,1],[1,1],[1,0],[0,0]]] }"
                                                  "}" );
    GWSAgent* sheep1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json1.object() ) );


    qDebug()<< "I am a GWSAgent of" << sheep1->property("@type").toString() << "type.";

    GWSBehaviour* b1 = new GWSBehaviour( sheep1 );
    TesisBehaviour* b11 = new TesisBehaviour( sheep1 );
    TesisBehaviour* b12 = new TesisBehaviour( sheep1 );
    sheep1->setStartBehaviour( b1 );
    b1->addSubbehaviour( b11 );
    b1->addSubbehaviour( b12 );

    TesisBehaviour* b2 = new TesisBehaviour( sheep1 );

    b1->setNextBehaviour( b2 );


    /* Dolly2 */
    QJsonDocument json2 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly2\" , "
                                                  "\"energy\" : 5 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [0 , 0 , 0]}"
                                                  "}" );
    GWSAgent* sheep2 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json2.object() ) );
    qInfo()<< "I am a GWSAgent of" << sheep2->property("@type").toString() << "type.";


    /* Dolly3 */
    QJsonDocument json3 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly3\" , "
                                                  "\"energy\" : 20 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [0 , 0 , 0]}"
                                                  "}" );
    GWSAgent* sheep3 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json3.object() ) );
    qInfo()<< "I am a GWSAgent of" << sheep3->property("@type").toString() << "type.";

    /* ----------
    /* WolfAgents
       ----------*/

    /* Nymeria1 */
    QJsonDocument json4 = QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria1\" , "
                                                  "\"energy\" : 20 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [2 , 2 , 0]}"
                                                  "}" );
    GWSAgent* predator1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json4.object() ) );
    qInfo()<< "I am a GWSAgent of" << predator1->property("@type").toString() << "type.";


    /* Nymeria2 */
    QJsonDocument json5 = QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria2\" , "
                                                  "\"energy\" : 15 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [1 , 2 , 0]}"
                                                  "}" );
    GWSAgent* predator2 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json5.object() ) );
    qInfo()<< "I am a GWSAgent of" << predator2->property("@type").toString() << "type.";

    /* Nymeria3 */
    QJsonDocument json6= QJsonDocument::fromJson( "{ \"@type\" : \"PredatorAgent\" , "
                                                  "\"@id\" : \"Nymeria3\" , "
                                                  "\"energy\" : 10 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [-2 , -2 , 0]}"
                                                  "}" );
    GWSAgent* predator3 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json6.object() ) );
    qInfo()<< "I am a GWSAgent of" << predator3->property("@type").toString() << "type.";


    // Register Agents in environments
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep2 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( sheep3 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator2 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( predator3 );
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
