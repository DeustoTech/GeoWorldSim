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



QJsonDocument human_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", \"@family\" : [\"Person\"]  , "
                                                        "\"running\" : true, \"color\" : \"Green\" , "
                                                        "\"@behaviours\" : [  { \"start\": true, \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1 , \"x_value\": \"<from_x>\", \"y_value\": \"<from_y>\", \"nexts\" : [\"MOVE\"] }, "
                                                                                  "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE\" , \"duration\" : 1 , \"maxspeed\" : 50 , \"x_value\": \"<to_x>\" , \"y_value\": \"<to_y>\" , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"store_current_speed_as\" : \"current_speed\", \"store_current_road_type_as\" : \"current_road_type\" , \"store_current_road_maxspeed\" : \"current_road_maxspeed\" , \"nexts_if_arrived\" : [\"STOP\"] , \"nexts_if_not_arrived\" : [\"DISPLAY\"] } , "
                                                                                  "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE\"] } ,"
                                                                                  "{ \"@type\": \"StopAgentBehaviour\" ,   \"@id\" : \"STOP\" , \"duration\" : 1 }  "
                                                                                  " ] } ")
                                                        .toLatin1()
                                                        );

QString url_censo_kg_resto = "http://datasources.geoworldsim.com/api/datasource/4ac4c9d1-f1d6-40e6-a286-2f1c7e8ed34a/read";
GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( human_json.object() , url_censo_kg_resto , 1000  );



/* ----------------
 * Bilbao roads
 * ----------------*/

// Read Primary Road data from datasource url:

QJsonDocument road_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\" , \"@family\" : [\"Road\"] , \"color\" : \"Blue\" , \"weight\" : 5 , \"maxspeed\" : 40 , "
                                                            "\"edge\" : { \"@type\" : \"GWSGraphEdge\" , \"edge_capacity\" : 3 } , "
                                                            "\"@behaviours\" : [ "
                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"duration\": 30 ,  \"start\": true, \"nexts\" : [\"CHANGE_COLOR\"] } , "
                                                            "{ \"@type\": \"ChangeColorRangeBehaviour\" ,   \"@id\": \"CHANGE_COLOR\" , \"min_capacity\" : 0 , \"max_capacity\" : 10 , \"duration\": 1 ,  \"nexts\" : [\"DISPLAY\"] }"
                                                            " ]"
                                                            " } ")
                                                        .toLatin1()
                                                        );

QString primary_road = "http://datasources.geoworldsim.com/api/datasource/92afa3e7-62fa-4ec3-803b-c60e93e63dcd/read" ;
GWSAgentGeneratorDatasource* ds1 = new GWSAgentGeneratorDatasource( road_json.object() , primary_road );


// Read Secondary Road data from datasource url:
QJsonDocument secondary_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\" } ")
                                                        .toLatin1()
                                                        );

QString secondary_road = "http://datasources.geoworldsim.com/api/datasource/a1c2a08b-9e16-4fdf-86d2-84c611c325fc/read" ;
GWSAgentGeneratorDatasource* ds2 = new GWSAgentGeneratorDatasource( road_json.object() , secondary_road );


// Read Tertiary Road data from datasource url:
QString tertiary_road = "http://datasources.geoworldsim.com/api/datasource/f2e32381-f035-49d9-8836-195ed020a26d/read";
GWSAgentGeneratorDatasource* ds3 = new GWSAgentGeneratorDatasource( road_json.object() , tertiary_road );


// Read Trunk Road data from datasource url:
QString trunk_road = "http://datasources.geoworldsim.com/api/datasource/a0d53dac-d0da-496e-9231-2cf526992429/read";
GWSAgentGeneratorDatasource* ds4 = new GWSAgentGeneratorDatasource( road_json.object() , trunk_road );

// Read Residential Road data from datasource url:
QString residential_road = "http://datasources.geoworldsim.com/api/datasource/ecd490dd-a01d-4eb7-b471-01c90a281cb7/read";
GWSAgentGeneratorDatasource* ds5 = new GWSAgentGeneratorDatasource( road_json.object() , residential_road );

/*GWSDatasourceReader* secondaryHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/a1c2a08b-9e16-4fdf-86d2-84c611c325fc/read" );


secondaryHighwayReader->connect( secondaryHighwayReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

    // If data.value( "oneway" ).toString() = "no" or data.value( "oneway" ).toString().isNull(), we need to generate the edges in two directions,
    // the same as we do for W4T roads

    if ( data.value( "oneway" ).toString() == "no" || data.value( "oneway" ).toString().isNull() ){

        try {
        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().last().toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().last().toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geometry" , geo);
            agent_json.insert( "edge" , edge );
            QJsonArray family_array; family_array.append( "secondary" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> secondary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( secondary->serialize() );
        }
        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");
            QJsonArray reversed_coordinates;
            for(int i = geo.value( "coordinates" ).toArray().size()-1 ; i >= 0 ; i-- ){
                reversed_coordinates.append( geo.value( "coordinates" ).toArray().at(i) );
            }
            geo.insert( "coordinates" , reversed_coordinates );

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().last().toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().last().toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geometry" , geo );
            agent_json.insert( "edge" , edge );
            QJsonArray family_array; family_array.append( "secondary" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> secondary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( secondary->serialize() );
        }

        }
        catch (std::exception &e){
        }

    }


    else {

        try {
        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().last().toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().last().toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geometry" , geo);
            agent_json.insert( "edge" , edge );
            QJsonArray family_array; family_array.append( "secondary" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> secondary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( secondary->serialize() );

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();
        }
        }
        catch (std::exception &e){
        }

    }

});*/



QTimer::singleShot( 10*1000 , [](){

    GWSTimeEnvironment::globalInstance()->setDatetime( 1000 );
    GWSExecutionEnvironment::globalInstance()->run();
} );


app->exec();

}
