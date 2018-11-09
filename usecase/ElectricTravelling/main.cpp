 #include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QtMath>

#include "../../agent/Agent.h"

// Agents

// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../skill/move/drive/DriveSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/electricTravelling/ChangeColorRangeBehaviour.h"
#include "../../behaviour/electricTravelling/DriveBehaviour.h"
#include "../../behaviour/execution/StopAgentBehaviour.h"

//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/datasource/AgentGeneratorDatasource.h"
#include "../../util/random/UniformDistribution.h"

#include <time.h>
#include <iostream>
#include <vector>


int main(int argc, char* argv[])
{
    QDateTime start = QDateTime::currentDateTime();
    qDebug() << start;
    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // Init used environments
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();

    // Agents


    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ChangeColorRangeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( DriveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject );

    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


/* --------------
 *  Agents
 *---------------*/



QJsonDocument human_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", \"@family\" : [\"Person\"]  , \"from_changed_x\" : -2.8631781 , \"from_changed_y\" : 43.2830898 , "
                                                        "\"running\" : true, \"color\" : \"Blue\" , "
                                                        "\"@behaviours\" : [  { \"start\": true, \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1 , \"x_value\": \"<from_changed_x>\", \"y_value\": \"<from_changed_y>\", \"nexts\" : [\"MOVE\"] }, "
                                                                                  "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE\" , \"duration\" : 1 , \"maxspeed\" : 50 , \"x_value\": \"<to_x>\" , \"y_value\": \"<to_y>\" , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"store_current_speed_as\" : \"current_speed\", \"store_current_road_type_as\" : \"current_road_type\" , \"store_current_road_maxspeed\" : \"current_road_maxspeed\" , \"nexts_if_arrived\" : [\"STOP\"] , \"nexts_if_not_arrived\" : [\"DISPLAY\"] } , "
                                                                                  "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE\"] } ,"
                                                                                  "{ \"@type\": \"StopAgentBehaviour\" ,   \"@id\" : \"STOP\" , \"duration\" : 1 }  "
                                                                                  " ] } ")
                                                        .toLatin1()
                                                        );

QString url_censo_kg_resto = "http://datasources.geoworldsim.com/api/datasource/5742d86c-f3a6-4f92-8a8a-52c77d2cef3a/read";
GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( human_json.object() , url_censo_kg_resto );



/* ----------------
 * Zamudio roads
 * ----------------*/

// Read Footway data from datasource url:

QJsonDocument road_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\" , \"@family\" : [\"Road\"] , \"color\" : \"Red\" , \"weight\" : 5 , \"maxspeed\" : 4 , "
                                                            "\"edge\" : { \"@type\" : \"GWSGraphEdge\" , \"capacity\" : 3 } , "
                                                            "\"@behaviours\" : [ "
                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"duration\": 30 ,  \"start\": true, \"nexts\" : [\"CHANGE_COLOR\"] } , "
                                                            "{ \"@type\": \"ChangeColorRangeBehaviour\" ,   \"@id\": \"CHANGE_COLOR\" , \"min_capacity\" : 0 , \"max_capacity\" : 10 , \"duration\": 1 ,  \"nexts\" : [\"DISPLAY\"] }"
                                                            " ]"
                                                            " } ")
                                                        .toLatin1()
                                                        );

QString primary_road = "http://datasources.geoworldsim.com/api/datasource/5623ec42-56a3-46b2-afd6-27a74613bbde/read" ;
GWSAgentGeneratorDatasource* ds1 = new GWSAgentGeneratorDatasource( road_json.object() , primary_road );


// Read Pedestrian data from datasource url:
QString secondary_road = "http://datasources.geoworldsim.com/api/datasource/0204533e-104e-4878-ac56-79c8960da545/read" ;
GWSAgentGeneratorDatasource* ds2 = new GWSAgentGeneratorDatasource( road_json.object() , secondary_road );


// Read Residential data from datasource url:
//QString tertiary_road = "http://datasources.geoworldsim.com/api/datasource/92101b3d-faf6-46a0-97b6-ef48747c07ef/read";
//GWSAgentGeneratorDatasource* ds3 = new GWSAgentGeneratorDatasource( road_json.object() , tertiary_road );

QTimer::singleShot( 10*1000 , [](){

    GWSTimeEnvironment::globalInstance()->setDatetime( 1000 );
    GWSExecutionEnvironment::globalInstance()->run();
} );

GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
    QDateTime current_time = QDateTime::currentDateTime();
    qint64 secondsDiff = start.secsTo( current_time );
    qDebug() << QString("Simulation took %1 seconds, for %2 agents, at %3x speed")
                .arg( secondsDiff )
                .arg( GWSAgentEnvironment::globalInstance()->getAmount() )
                .arg( GWSTimeEnvironment::globalInstance()->getTimeSpeed() );
});

app->exec();


}
