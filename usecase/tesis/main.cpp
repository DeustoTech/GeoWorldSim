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

#include "SheepAgent.h"

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




    QJsonDocument json1 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"cell_x\" : 0 , "
                                                  "\"cell_y\" : 1 , "
                                                  "\"energy\" : 100 , "
                                                  "\"geo\" : { \"type\" : \"Point\" , \"coordinates\" : [0 , 0 , 0] }"
                                                  "}" );
    GWSAgent* agent1 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json1.object() ) );

    QVariant v1 = agent1->property("@type");
    qDebug()<< "I am Agent1 and I am of" << v1.toString() << "type, and my initial absize position is" << agent1->property("cell_x").toString();



    GWSBehaviour* b1 = new GWSBehaviour( agent1 );
    TesisBehaviour* b11 = new TesisBehaviour( agent1 );
    TesisBehaviour* b12 = new TesisBehaviour( agent1 );
    agent1->setStartBehaviour( b1 );
    b1->addSubbehaviour( b11 );
    b1->addSubbehaviour( b12 );

    TesisBehaviour* b2 = new TesisBehaviour( agent1 );

    b1->setNextBehaviour( b2 );



    // Add second agent
    QJsonDocument json2 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly2\" , "
                                                  "\"cell_x\" : 1 , "
                                                  "\"cell_y\" : 0 , "
                                                  "\"energy\" : 5 , "
                                                  "\"geo\" : { \"@type\" : \"GWSPoint\" } , "
                                                  "\"grid\" : { \"@type\": \"GWSGrid\" , "
                                                                "\"values\" : [ [1,2] , [3,4]] ,  "
                                                                "\"max_value\": 100 , "
                                                  "             \"min_value\" : 0 } "
                                                  "}" );
    GWSAgent* agent2 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json2.object() ) );

    QVariant v2 = agent2->property("@type");
    qDebug()<< "I am Agent2 and I am of" << v2.toString() << "type, and my initial absize position is" << agent2->property("cell_x").toString();


    // Add third agent
    QJsonDocument json3 = QJsonDocument::fromJson( "{ \"@type\" : \"SheepAgent\" , "
                                                  "\"@id\" : \"Dolly3\" , "
                                                  "\"cell_x\" : 0 , "
                                                  "\"cell_y\" : 0 , "
                                                  "\"energy\" : 20 , "
                                                  "\"geo\" : { \"@type\" : \"GWSPoint\" } , "
                                                  "\"grid\" : { \"@type\": \"GWSGrid\" , "
                                                                "\"values\" : [ [1,2] , [3,4]] ,  "
                                                                "\"max_value\": 100 , "
                                                  "             \"min_value\" : 0 } "
                                                  "}" );
    GWSAgent* agent3 = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( json3.object() ) );

    QVariant v3 = agent3->property("@type");
    qDebug()<< "I am Agent3 and I am of" << v3.toString() << "type, and my initial absize position is" << agent3->property("cell_x").toString();





    // Register in environments
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent1 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent2 );
    GWSExecutionEnvironment::globalInstance()->registerAgent( agent3 );
    GWSAgentEnvironment::globalInstance()->registerAgent( agent1 );
    GWSAgentEnvironment::globalInstance()->registerAgent( agent2 );
    GWSAgentEnvironment::globalInstance()->registerAgent( agent3 );

    QList<GWSAgent*> l = GWSAgentEnvironment::globalInstance()->getByClass<GWSAgent>( GWSAgent::staticMetaObject.className() );

    GWSExecutionEnvironment::globalInstance()->run();





    app->exec();


    /* Generate subroutine for this method so that it can be generalized to all agents present.
     *

     Store all present agents in an array
    GWSAgent* SheepFlock[3] = {agent1, agent2, agent3};
    for (int i = 0; i <= 3; i++)
        {
        for (int j = 0; j <= 3 ; j++ )
            {
            if (
               (SheepFlock[i]->property("cell_x") == SheepFlock[j]->property("cell_x"))
                &&
               (SheepFlock[i]->property("cell_y") == SheepFlock[j]->property("cell_y"))
                &&
               (i != j)
               )
               {
               qDebug() << "This cell is too crowded!";
               }
           else
               {
               qDebug() << "Not overbooked yet!";
               }
            }
        }
   */

}
