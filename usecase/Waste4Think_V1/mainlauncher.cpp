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
#include "../../behaviour/waste4think/GenerateRandomValueBehaviour.h"
#include "../../behaviour/waste4think/PolluteBehaviour.h"
#include "../../behaviour/waste4think/ChooseRandomValueFromSetBehaviour.h"
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
#include "../../util/io/csv/CsvImporter.h"

int main(int argc, char* argv[])
{

    QDateTime start = QDateTime::currentDateTime();

    // CREATE QAPPLICATION
    GWSApp* app = GWSApp::globalInstance( argc , argv );

    // INIT ENVIRONMENTS
    GWSObjectFactory::globalInstance();
    GWSAgentEnvironment::globalInstance();
    GWSExecutionEnvironment::globalInstance();
    GWSPhysicalEnvironment::globalInstance();
    GWSNetworkEnvironment::globalInstance();
    GWSTimeEnvironment::globalInstance();

    // AVAILABLE AGENTS
    GWSObjectFactory::globalInstance()->registerType( ContainerAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TruckAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( RecyclingPlantAgent::staticMetaObject);

    // AVAILABLE BEHAVIOURS
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
    GWSObjectFactory::globalInstance()->registerType( GenerateRandomValueBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( PolluteBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( ChooseRandomValueFromSetBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( StopAgentBehaviour::staticMetaObject ) ;

    // INIT RANDOM NUMBERS
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // READ CONFIGURATION
    QFile file( app->property( "config" ).toString() );
    QJsonObject json_configuration;
    if( !file.open( QFile::ReadOnly ) ){
        qCritical() << QString("No configuration file found");
        return -1;
    }
    QJsonParseError jerror;
    json_configuration = QJsonDocument::fromJson( file.readAll() , &jerror ).object();
    if( jerror.error != QJsonParseError::NoError ){
        qCritical() << QString("Error when parsing configuration JSON: %1").arg( jerror.errorString() );
        return -1;
    }

    // CREATE POPULATION
    int pending_population = 0;
    QJsonObject json_population = json_configuration.value("population").toObject();
    foreach( QString key , json_population.keys() ) {

        // Population type:
        QJsonObject population = json_population[ key ].toObject();

        if ( !population.value("template").isNull() && !population.value("datasource_url").isNull() ){
            pending_population++;
            GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( population.value("template").toObject() , population.value("datasource_url").toString() );
            ds->connect( ds , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [ds,&pending_population](){
                pending_population--;
                ds->deleteLater();
                if( pending_population <= 0 ){
                    GWSExecutionEnvironment::globalInstance()->run();
                }
            });
        }

        if ( !population.value("template").isNull() && !population.value("amount").isNull() ){
            for ( int i = 0; i < population.value("amount").toInt() ; i++){
                // Use template to generate amount agents
                GWSObjectFactory::globalInstance()->fromJSON( population.value("template").toObject() ).dynamicCast<GWSAgent>();
            }
        }
       qDebug() << QString("Creating population %1").arg( key );
    }
    if( pending_population <= 0 ){
        GWSExecutionEnvironment::globalInstance()->run();
    }




    // LISTEN TO EXTERNAL SIMULATIONS
    QJsonObject json_external_listeners = json_configuration.value("external_listeners").toObject();
    foreach( QString key , json_external_listeners.keys() ) {

        // Get simulation to be listened to from config.json file
        if ( !json_external_listeners[ key ].isNull() ){
            new GWSExternalListener( json_external_listeners[ key ].toString() );
        }
        qDebug() << QString("Creating external listener %1").arg( key );
     }



    /* ----------------
     * Human Agents
     * ----------------*/

// Agents move:
/*QJsonDocument human_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"HumanAgent\",  "
                                                        "\"@family\": [ \"GWSAgent\", \"Citizen\" ], "
                                                        "\"home_x\": -2, \"home_y\": 43, \"color\" : \"Gray\" ,  \"running\" : true , "
                                                        "\"@behaviours\": [  { \"@type\": \"GenerateAgentGeometryBehaviour\", \"start\" : true , \"@id\": \"GEOM\", \"duration\": 1, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"nexts\" : [\"DISPLAY_AT_HOME\"] }, "
                                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_AT_HOME\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 60 ,  \"nexts\" : [\"WASTE\"] } ,"
                                                                            "{ \"@type\": \"GenerateWasteZamudioModelBehaviour\", \"@id\": \"WASTE\", \"duration\": 1, \"waste_type1\" : \"resto\" , \"store_waste_type1_as\": \"resto\", \"nexts\" : [\"FIND\"] }, "
                                                                            "{ \"@type\": \"FindClosestBehaviour\", \"duration\": 1, \"@id\": \"FIND\", \"closest_agent_type\": \"ContainerAgent\", \"transport_network_type\": \"Road\", \"store_closest_id_as\": \"closest_agent_id\" , \"store_closest_x_as\": \"closest_agent_x\", \"store_closest_y_as\": \"closest_agent_y\" , \"store_closest_route_distance_as\": \"closest_container_distance\" , \"nexts\": [ \"MOVE_TO_CONTAINER\" ] }, "
                                                                            "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_TO_CONTAINER\" , \"duration\" : 1 , \"maxspeed\" : 50 , \"x_value\": \"<closest_agent_x>\" , \"y_value\": \"<closest_agent_y>\" , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"store_current_speed_as\" : \"current_speed\", \"store_current_road_type_as\" : \"current_road_type\" , \"store_current_road_maxspeed\" : \"current_road_maxspeed\" , \"nexts_if_arrived\" : [\"COPY\"] , \"nexts_if_not_arrived\" : [\"DISPLAY_MOVING_TO_CONTAINER\"] } , "
                                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_MOVING_TO_CONTAINER\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE_TO_CONTAINER\"] } ,"
                                                                            "{ \"@type\": \"CopyPropertyBehaviour\", \"duration\": 1, \"@id\": \"COPY\", \"agent_id_to_copy_from\": \"<closest_agent_id>\", \"property_name\" : \"color\" , \"nexts\": [ \"TRANSFER\" ] },  "
                                                                            "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER\", \"property_name_to_transfer\": \"resto\" , \"emitting_agent_id\" : \"<@id>\" , \"receiving_agent_id\": \"<closest_agent_id>\" , \"nexts\" : [\"MOVE_BACK_HOME\"] }, "
                                                                            "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_BACK_HOME\" , \"duration\" : 1 , \"maxspeed\" : 50 , \"x_value\": \"<X>\" , \"y_value\": \"<Y>\" , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"store_current_speed_as\" : \"current_speed\", \"store_current_road_type_as\" : \"current_road_type\" , \"store_current_road_maxspeed\" : \"current_road_maxspeed\" , \"nexts_if_arrived\" : [\"DISPLAY_AT_HOME\"] , \"nexts_if_not_arrived\" : [\"DISPLAY_MOVING_BACK_HOME\"] } , "
                                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_MOVING_BACK_HOME\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE_BACK_HOME\"] } "
                                                        "] } ")
                                                        .toLatin1()
                                                        );*/

// Agents don't move:
/*QJsonDocument human_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"HumanAgent\",  "
                                                        "\"@family\": [ \"GWSAgent\", \"Citizen\" ], "
                                                        "\"home_x\": -2, \"home_y\": 43, \"color\" : \"Gray\" ,  \"running\" : true , "
                                                        "\"@behaviours\": [  { \"@type\": \"GenerateAgentGeometryBehaviour\", \"start\" : true , \"@id\": \"GEOM\", \"duration\": 1, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"nexts\" : [\"DISPLAY_AT_HOME\"] }, "
                                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_AT_HOME\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 60 ,  \"nexts\" : [\"WASTE\"] } ,"
                                                                            "{ \"@type\": \"GenerateWasteZamudioModelBehaviour\", \"@id\": \"WASTE\", \"duration\": 1, \"waste_type1\" : \"resto\" , \"store_waste_type1_as\": \"resto\", \"nexts\" : [\"FIND\"] }, "
                                                                            "{ \"@type\": \"FindClosestBehaviour\", \"duration\": 1, \"@id\": \"FIND\", \"closest_agent_type\": \"ContainerAgent\", \"transport_network_type\": \"Road\", \"store_closest_id_as\": \"closest_agent_id\", \"store_closest_x_as\": \"closest_agent_x\", \"store_closest_y_as\": \"closest_agent_y\", \"store_closest_route_distance_as\": \"closest_container_distance\", \"nexts\": [ \"COPY\" ] }, "
                                                                            "{ \"@type\": \"CopyPropertyBehaviour\", \"duration\": 1, \"@id\": \"COPY\", \"agent_id_to_copy_from\": \"<closest_agent_id>\", \"property_name\" : \"color\" , \"nexts\": [ \"TRANSFER\" ] },  "
                                                                            "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER\", \"property_name_to_transfer\": \"resto\" , \"emitting_agent_id\" : \"<@id>\" , \"receiving_agent_id\": \"<closest_agent_id>\" , \"nexts\" : [\"DISPLAY_AT_HOME\"] } "
                                                                           "] } ")
                                                        .toLatin1()
                                                        );



QString url_censo_kg_resto = "http://datasources.geoworldsim.com/api/datasource/098a0eb1-9504-41af-9a93-fcb6bfc772d8/read";
//GWSAgentGeneratorDatasource* ds = new GWSAgentGeneratorDatasource( human_json.object() , url_censo_kg_resto  );
*/

/* ----------------
 * Truck Agents
 * ----------------*/

/*for( int i = 0 ; i < 0 ; i++ ){
//    "{ \"@type\": \"FollowTSPRouteBehaviour\" , \"@id\" : \"FOLLOW_TSP\" , \"duration\" : 1 , \"tsp_route\" : \"<tsp_route>\", \"tsp_route_stage\" : 0, , \"nexts\" : [\"MOVE_STAGES\"] } ,"

    QJsonDocument jsonTrucks = QJsonDocument::fromJson( QString("{ \"@type\" : \"TruckAgent\" , "
                                                                  "\"@family\": [ \"GWSAgent\" ], "
                                                                  "\"home_x\" :  %1, \"home_y\" :  %2,  \"weight\": 10 , \"color\" : \"Black\" , \"running\" : true , "
                                                                  "\"@behaviours\" : [ { \"@type\": \"GenerateAgentGeometryBehaviour\", \"start\" : true , \"@id\": \"GEOM\", \"duration\": 1 , \"x_value\": %1,  \"y_value\": %2, \"nexts\" : [\"DISPLAY\"] }, "
                                                                                       "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"TSP\"] } ,"
                                                                                       "{ \"@type\": \"CalculateTSPRouteBehaviour\" , \"@id\" : \"TSP\" , \"duration\" : 1 , \"tsp_agent_type\" : \"ContainerAgent\" , \"transport_network_type\": \"Road\",  \"store_tsp_route_as\" : \"tsp_route\" , \"store_closest_id_as\" : \"closest_container_id\" , \"store_tsp_route_stage_x_as\" : \"x_dest\", \"store_tsp_route_stage_y_as\" : \"y_dest\" , \"nexts_if_no_tsp_route\" : [\"TSP\"] , \"nexts_if_tsp_route\" : [\"MOVE_STAGES\"] } , "
                                                                                       "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_STAGES\" , \"duration\" : 1 , \"maxspeed\" : 150 , \"x_value\": \"<x_dest>\", \"y_value\": \"<y_dest>\", \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"nexts_if_arrived\" : [\"COPY\"] , \"nexts_if_not_arrived\" : [\"DISPLAY_MOVING\"] } , "
                                                                                       "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_MOVING\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE_STAGES\"] } ,"
                                                                                       "{ \"@type\": \"CopyPropertyBehaviour\", \"duration\": 1, \"@id\": \"COPY\", \"agent_id_to_copy_from\": \"<closest_container_id>\", \"property_name\" : \"color\" , \"nexts\": [ \"TRANSFER_CONTAINER_TO_TRUCK\" ] }, "
                                                                                       "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER_CONTAINER_TO_TRUCK\", \"property_name_to_transfer\": \"resto\", \"emitting_agent_id\": \"<closest_container_id>\" , \"receiving_agent_id\":  \"<@id>\", \"nexts\" : [\"CHECK_WASTE\"] } ,"
                                                                                       "{ \"@type\": \"CheckPropertyValueBehaviour\", \"duration\": 1, \"@id\": \"CHECK_WASTE\", \"property_to_compare\": \"resto\", \"threshold_value\": \"100000\" , \"nexts_if_true\" : [\"MOVE_PLANT\"] , \"nexts_if_false\" : [\"TSP\"] } ,"
                                                                                       "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_PLANT\" , \"duration\" : 1 , \"maxspeed\" : 40 , \"x_value\": %1, \"y_value\": %2 , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"nexts_if_arrived\" : [\"TRANSFER_PLANT\"] , \"nexts_if_not_arrived\" : [\"DISPLAY_MOVING_TO_PLANT\"] } , "
                                                                                       "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY_MOVING_TO_PLANT\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"MOVE_PLANT\"] } ,"
                                                                                       "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER_PLANT\", \"property_name_to_transfer\": \"resto\", \"emitting_agent_id\" : \"<@id>\" , \"receiving_agent_id\": \"RecyclingPlant\" , \"nexts\" : [\"TSP\"] } "
                                                                                       " ] } ")
                                                    .arg( -2.86390 )
                                                    .arg( 43.28509 )
                                                   .toLatin1()
                                                    );

    QSharedPointer<GWSAgent> truck = GWSObjectFactory::globalInstance()->fromJSON( jsonTrucks.object() ).dynamicCast<GWSAgent>();


}*/

/* ----------------------
 * RecyclingPlant Agents
 * ----------------------*/

/*QJsonDocument jsonPlant = QJsonDocument::fromJson( QString( "{ \"@type\": \"RecyclingPlantAgent\", \"@id\" : \"RecyclingPlant\" , \"color\" : \"Orange\" ,  \"weight\": 8 , \"running\" : true , "
                                                                 "\"geometry\" : { \"@type\" : \"GWSGeometry\" ,  \"type\" :  \"Point\" , \"coordinates\" : [ %1 , %2 , 0 ] }  , "
                                                                 "\"@family\": [ \"GWSAgent\" ]  ,"
                                                                 "\"@behaviours\" : [ "
                                                                                      "{ \"@type\": \"GenerateRandomValueBehaviour\", \"duration\" : 1 , \"start\" : true , \"@id\" : \"RANDOM_VALUE\" , \"store_random_value_as\" : \"random_value\"  , \"nexts\" : [\"POLLUTE\"] } ,"
                                                                                      "{ \"@type\" : \"PolluteBehaviour\" , \"@id\" : \"POLLUTE\" , \"duration\" : 1  , \"nexts\" : [\"DISPLAY\"] } , "
                                                                                      "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 0 ,  \"nexts\" : [\"RANDOM_VALUE\"] } "
                                                                                      "] }")
                                                               .arg( -2.86390 )
                                                               .arg( 43.28509 )
                                                               .toLatin1()
                                                                 );*/
//QSharedPointer<GWSAgent> plant = GWSObjectFactory::globalInstance()->fromJSON( jsonPlant.object() ).dynamicCast<GWSAgent>();



/* ----------------
 * Container Agents
 * ----------------*/

// Read container data from datasource url:

/*QJsonDocument container_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", "
                                                                 "\"weight\": 5 , \"geometry\" : { \"@type\" : \"GWSGeometry\" } , "
                                                                 "\"@family\": [ \"ContainerAgent\", \"Container\" ] } ")
                                                                 .toLatin1()
                                                                 );
QString container_url = "http://datasources.geoworldsim.com/api/datasource/efd5cf54-d737-4866-9ff3-c82d129ea44b/read";
GWSAgentGeneratorDatasource* ds_container = new GWSAgentGeneratorDatasource( container_json.object() , container_url );
*/



/* ----------------
 * Zamudio roads
 * ----------------*/

/*QJsonDocument road_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\" , \"@family\" : [\"Road\"] , \"color\" : \"Blue\" , \"weight\" : 5 , \"maxspeed\" : 4 , \"running\" : true ,"
                                                            "\"edge\" : { \"@type\" : \"GWSGraphEdge\" , \"capacity\" : 3 } , "
                                                            "\"@behaviours\" : [ "
                                                            "{ \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"DISPLAY\" , \"agent_to_send_id\" : \"<@id>\" , \"duration\": 30 ,  \"start\": true, \"nexts\" : [\"DISPLAY\"] }  "
                                                            " ] } ")
                                                        .toLatin1()
                                                        );

// Read PEDESTRIAN ROAD data from datasource url:
QString pedestrian_reader = "http://datasources.geoworldsim.com/api/datasource/0204533e-104e-4878-ac56-79c8960da545/read" ;
GWSAgentGeneratorDatasource* ds1 = new GWSAgentGeneratorDatasource( road_json.object() , pedestrian_reader );


// Read MISSING ROAD data from datasource url:
QString missing_reader = "http://datasources.geoworldsim.com/api/datasource/92101b3d-faf6-46a0-97b6-ef48747c07ef/read" ;
GWSAgentGeneratorDatasource* ds2 = new GWSAgentGeneratorDatasource( road_json.object() , missing_reader );


// Read FOOTWAY ROAD data from datasource url:
QString footway_reader = "http://datasources.geoworldsim.com/api/datasource/5623ec42-56a3-46b2-afd6-27a74613bbde/read" ;
GWSAgentGeneratorDatasource* ds3 = new GWSAgentGeneratorDatasource( road_json.object() , footway_reader );



ds3->connect(ds3 , &GWSAgentGeneratorDatasource::dataReadingFinishedSignal , [](){

    foreach (QSharedPointer<GWSAgent> a , GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className() ) ) {
        a->setProperty( "color" , QColor::colorNames().at( qrand() % QColor::colorNames().size() ) );
    }
    GWSExecutionEnvironment::globalInstance()->run();
} );*/

GWSExecutionEnvironment::connect( GWSExecutionEnvironment::globalInstance() , &GWSExecutionEnvironment::stoppingExecutionSignal , [start]( ){
    QDateTime current_time = QDateTime::currentDateTime();
    qint64 secondsDiff = start.secsTo( current_time );
    qDebug() << "Elapsed time" << secondsDiff;
});



app->exec();



}
