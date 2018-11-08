 #include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QtMath>

#include "../../agent/Agent.h"

// Agents
#include "ContainerAgent.h"
#include "HumanAgent.h"
#include "TruckAgent.h"
#include "RecyclingPlantAgent.h"


// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/waste4think/DelayBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteZamudioModelBehaviour.h"
#include "../../behaviour/waste4think/FindClosestBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/waste4think/FollowTSPRouteBehaviour.h"
#include "../../behaviour/move/CalculateTSPRouteBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/information/SendAgentSnapshotBehaviour.h"
#include "../../behaviour/waste4think/GatherAgentPropertyBehaviour.h"
#include "../../behaviour/property/CopyPropertyBehaviour.h"
#include "../../behaviour/waste4think/CheckPropertyValueBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( ContainerAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TruckAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( RecyclingPlantAgent::staticMetaObject);

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteZamudioModelBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( DelayBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransferAgentPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CalculateTSPRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FollowTSPRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SendAgentSnapshotBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( GatherAgentPropertyBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( CopyPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckPropertyValueBehaviour::staticMetaObject);


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    /* ----------------
     * Human Agents
     * ----------------*/

QJsonDocument human_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"HumanAgent\",  "
                                                        "\"@family\": [ \"GWSAgent\", \"Citizen\" ], "
                                                        "\"home_x\": -2, \"home_y\": 43, \"running\" : true, \"color\" : \"Green\" , "
                                                        "\"@behaviours\": [ { \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"HISTORY\" , \"duration\": 1 , \"start\": true, \"nexts\" : [\"GEOM\"] } ,"
                                                                            "{ \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"nexts\" : [\"WASTE\"] }, "
                                                                            "{ \"@type\": \"GenerateWasteZamudioModelBehaviour\", \"@id\": \"WASTE\", \"duration\": 10, \"waste_type1\" : \"despilfarro_alimento\" , \"store_waste_type1_as\": \"despilfarro_alimento\", \"nexts_if_true\" : [\"WAIT\"] }, "
                                                                            "{ \"@type\": \"DelayBehaviour\", \"@id\": \"WAIT\", \"duration\": 1,  \"nexts\" : [\"FIND\"] }, "
                                                                            "{ \"@type\": \"FindClosestBehaviour\", \"duration\": 1, \"@id\": \"FIND\", \"closest_agent_type\": \"ContainerAgent\", \"transport_network_type\": \"Road\", \"store_closest_id_as\": \"closest_container_id\", \"store_closest_route_distance_as\": \"closest_container_distance\", \"nexts\": [ \"COPY\" ] }, "
                                                                            "{ \"@type\": \"CopyPropertyBehaviour\", \"duration\": 1, \"@id\": \"COPY\", \"agent_id_to_copy_from\": \"<closest_container_id>\", \"property_name\" : \"color\" , \"nexts\": [ \"TRANSFER\" ] },  "
                                                                            "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER\", \"property_to_transfer\": \"<despilfarro_alimento>\", \"receiving_agent_id\": \"<closest_container_id>\" , \"nexts\" : [\"HISTORY\"] } "
                                                        "] } ").arg( 60 + qrand() % 60 )
                                                        .toLatin1()
                                                        );

QString url_censo_kg_resto = "http://datasources.geoworldsim.com/api/datasource/5742d86c-f3a6-4f92-8a8a-52c77d2cef3a/read";
GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( human_json.object() , url_censo_kg_resto , 1 );


/* ----------------
 * Truck Agents
 * ----------------*/


/* Returns a random double between min and max
 Zamudio latitude = 43.2803457
 Zamudio longitude = -2.8621286 */
double lat_max = 43.28472587082224;
double lat_min = 43.280961278501344;
double lon_max = -2.859949952301804 ;
double lon_min = -2.8665803729866184;


for( int i = 0 ; i < 0 ; i++ ){

    QJsonDocument jsonTrucks = QJsonDocument::fromJson( QString("{ \"@type\" : \"TruckAgent\" , "
                                                                  "\"@family\": [ \"GWSAgent\" ], \"running\" : true, "
                                                                  "\"home_x\" :  %1, \"home_y\" :  %2,  \"color\" : \"Blue\" , "
                                                                  "\"@behaviours\" : [  { \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"HISTORY\" , \"duration\": 1 , \"start\": true, \"nexts\" : [\"GEOM\"] } ,"
                                                                                       "{ \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1 , \"x_value\": %1,  \"y_value\": %2, \"nexts\" : [\"TSP\"] }, "
                                                                                       "{ \"@type\": \"CalculateTSPRouteBehaviour\" , \"@id\" : \"TSP\" , \"duration\" : 1 , \"tsp_agent_type\" : \"ContainerAgent\" , \"transport_network_type\": \"Road\",  \"store_tsp_route_as\" : \"tsp_route\", \"nexts\" : [\"FOLLOW_TSP\"] } , "
                                                                                       "{ \"@type\": \"FollowTSPRouteBehaviour\" , \"@id\" : \"FOLLOW_TSP\" , \"duration\" : 1 , \"tsp_route\" : \"<tsp_route>\", \"tsp_route_stage\" : 0, \"store_closest_id_as\": \"closest_container_id\" , \"store_tsp_route_stage_x_as\" : \"x_dest\", \"store_tsp_route_stage_y_as\" : \"y_dest\", \"nexts\" : [\"MOVE_STAGES\"] } ,"
                                                                                       "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_STAGES\" , \"duration\" : 1 , \"maxspeed\" : 150 , \"x_value\": \"<x_dest>\", \"y_value\": \"<y_dest>\", \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"nexts_if_arrived\" : [\"COPY\"] , \"nexts_if_not_arrived\" : [\"MOVE_STAGES\"] } , "
                                                                                       "{ \"@type\": \"CopyPropertyBehaviour\", \"duration\": 1, \"@id\": \"COPY\", \"agent_id_to_copy_from\": \"<closest_container_id>\", \"property_name\" : \"color\" , \"nexts\": [ \"GATHER\" ] }, "
                                                                                       "{ \"@type\": \"GatherAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"GATHER\", \"gather_property\": \"waste\", \"emitting_agent_id\": \"<closest_container_id>\" , \"nexts\" : [\"CHECK_WASTE\"] } ,"
                                                                                       "{ \"@type\": \"CheckPropertyValueBehaviour\", \"duration\": 1, \"@id\": \"CHECK_WASTE\", \"property_to_compare\": \"<waste>\", \"threshold_value\": \"10000\" , \"nexts_if_true\" : [\"MOVE_PLANT\"] , \"nexts_if_false\" : [\"FOLLOW_TSP\"] } ,"
                                                                                       "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_PLANT\" , \"duration\" : 1 , \"maxspeed\" : 40 , \"x_value\": %1, \"y_value\": %2 , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"nexts_if_arrived\" : [\"TRANSFER_PLANT\"] , \"nexts_if_not_arrived\" : [\"MOVE_PLANT\"] } , "
                                                                                       "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER_PLANT\", \"property_to_transfer\": \"waste\", \"receiving_agent_id\": \"RecyclingPlant\" , \"nexts\" : [\"FOLLOW_TSP\"] } "
                                                                                       " ] } ")
                                                   .arg( -2.8638 )
                                                   .arg( 43.2871 )
                                                   .toLatin1()
                                                    );

    QSharedPointer<GWSAgent> truck = GWSObjectFactory::globalInstance()->fromJSON( jsonTrucks.object() ).dynamicCast<GWSAgent>();


}

/* ----------------------
 * RecyclingPlant Agents
 * ----------------------*/

QJsonDocument jsonPlant = QJsonDocument::fromJson( QString( "{ \"@type\": \"RecyclingPlantAgent\", \"@id\" : \"RecyclingPlant\" , \"color\" : \"darkGreen\" ,  "
                                                                 "\"geometry\" : { \"@type\" : \"GWSGeometry\" ,  \"type\" :  \"Point\" , \"coordinates\" : [ %1 , %2 , 0 ] }  , "
                                                                 "\"@family\": [ \"GWSAgent\" ]  } ")
                                                               .arg( -2.8638 )
                                                               .arg( 43.2871 )
                                                               .toLatin1()
                                                                 );
QSharedPointer<GWSAgent> plant = GWSObjectFactory::globalInstance()->fromJSON( jsonPlant.object() ).dynamicCast<GWSAgent>();
qDebug() << GWSPhysicalEnvironment::globalInstance()->getGeometry( plant )->getCentroid().toString();

emit GWSApp::globalInstance()->sendAgentToSocketSignal( plant->serialize() );

/* ----------------
 * Container Agents
 * ----------------*/

// Read container data from datasource url:

QJsonDocument container_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", "
                                                                 "\"home_x\": -2, \"home_y\": 43, \"geometry\" : { \"@type\" : \"GWSGeometry\" } , "
                                                                 "\"@family\": [ \"ContainerAgent\", \"Container\" ] } ")
                                                                 .toLatin1()
                                                                 );

GWSAgentGeneratorDatasource* ds_container = new GWSAgentGeneratorDatasource( container_json.object() , "http://datasources.geoworldsim.com/api/datasource/efd5cf54-d737-4866-9ff3-c82d129ea44b/read" );




/* ----------------
 * Zamudio roads
 * ----------------*/


// Read PEDESTRIAN ROAD data from datasource url:
GWSDatasourceReader* pedestrian_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/0204533e-104e-4878-ac56-79c8960da545/read" );

pedestrian_reader->connect( pedestrian_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
        QJsonArray family_array; family_array.append( "Road" );
        agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( pedestrian->serialize() );
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
        QJsonArray family_array; family_array.append( "Road" );
        agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

        GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
        env->getId();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( pedestrian->serialize() );
    }

    }
    catch (std::exception &e){

    }

});

// Read MISSING ROAD data from datasource url:
GWSDatasourceReader* other_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/92101b3d-faf6-46a0-97b6-ef48747c07ef/read" );

other_reader->connect( other_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

    try {
    {
        QJsonObject geo = data.value( "geometry").toObject();
        geo.insert( "@type" ,  "GWSGeometry");

        QJsonObject edge;
        edge.insert( "@type" , "GWSGraphEdge" );
        edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
        edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
        edge.insert( "edge_to_x"   , geo.value( "coordinates" ).toArray().last( ).toArray().at( 0 ) );
        edge.insert( "edge_to_y"   , geo.value( "coordinates" ).toArray().last( ).toArray().at( 1 ) );

        QJsonObject agent_json;
        agent_json.insert( "geometry" , geo);
        agent_json.insert( "edge" , edge );
        //QJsonArray family_array; family_array.append( "Road" );
        //agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> other = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( other->serialize() );
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
        QJsonArray family_array; family_array.append( "Road" );
        agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> other = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

       // GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
       // env->getId();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( other->serialize() );
    }

    }
    catch (std::exception &e){

    }
});


// Read FOOTWAY ROAD data from datasource url:
GWSDatasourceReader* footway_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/5623ec42-56a3-46b2-afd6-27a74613bbde/read" );

footway_reader->connect( footway_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
        //QJsonArray family_array; family_array.append( "Road" );
        //agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( footway->serialize() );
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
        QJsonArray family_array; family_array.append( "Road" );
        agent_json.insert( "@family" , family_array );
        agent_json.insert( "@type" , "GWSAgent" );

        QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

        //GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
        //env->getId();

        emit GWSApp::globalInstance()->sendAgentToSocketSignal( footway->serialize() );
    }

    }
    catch (std::exception &e){

    }
});


footway_reader->connect( footway_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){


});

    footway_reader->startReading();
    pedestrian_reader->startReading();
    //other_reader->startReading();


    QTimer::singleShot( 10*1000 , [](){

        GWSTimeEnvironment::globalInstance()->setDatetime( 1000 );

        foreach (QSharedPointer<GWSAgent> a , GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className() ) ) {
            a->setProperty( "color" , QColor::colorNames().at( qrand() % QColor::colorNames().size() ) );
        }

        GWSExecutionEnvironment::globalInstance()->run();
    } );

    GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::tickEndedSignal , [](int executed_tick){

        qint64 dt = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();

       /* if( dt > 300 * 1000 ){
            qDebug() << executed_tick;
            GWSApp::globalInstance()->exit(1);
        }*/

    });

app->exec();

}
