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


// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/check/CheckIfAtPositionBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteBehaviour.h"


//Environments
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

// Utils
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../util/graph/Graph.h"
#include "../../util/routing/DijkstraRouting.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/routing/TSPRouting.h"
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
    GWSNetworkEnvironment::globalInstance();

    // Agents
    GWSObjectFactory::globalInstance()->registerType( ContainerAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( HumanAgent::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TruckAgent::staticMetaObject );

    // Skills
    GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteSkill::staticMetaObject );

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteBehaviour::staticMetaObject );

    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    /* Returns a random double between min and max
     Zamudio latitude = 43.2803457
     Zamudio longitude = -2.8621286 */
    double lat_max = 43.28472587082224;
    double lat_min = 43.280961278501344;
    double lon_max = -2.859949952301804 ;
    double lon_min = -2.8665803729866184;


    /* ----------------
     * Human Agents
     * ----------------*/
     // The random position generator will eventually be substituted by data from the census, similar to the procedure for containers

   /* for( int i = 0 ; i < 4; i++ ){

        QJsonDocument jsonHumans = QJsonDocument::fromJson( QString("{ \"@type\" : \"HumanAgent\" , "
                                                                    "\"waste_amount\" : 0 , "
                                                                    "\"home_coordX\" : %1 , "
                                                                    "\"home_coordY\" : %2 , "
                                                                    "\"color\" : \"Green\" , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 25 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/145/145852.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                            "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"start\" : true , \"@id\" : \"INCREMENT\" ,  \"property\" : \"waste_amount\" , \"increment\" : %3 , \"max\" : 100. , \"min\" : -1 , \"duration\" : 1000 , \"@next\" : \"SET_COLOR_FULL\" } , "
                                                                                            "{ \"@type\" : \"SetAgentPropertyBehaviour\" , \"@id\" : \"SET_COLOR_FULL\" , \"property_name\" : \"color\" , \"property_value\" : \"Red\" , \"duration\" : 1000 , \"@next\" : \"FIND_CLOSEST_CONTAINER\" } , "
                                                                                            "{ \"@type\" : \"FindClosestBehaviour\" , \"@id\" : \"FIND_CLOSEST_CONTAINER\" , \"duration\" : 1000 , \"@next\" : \"MOVE\" } , "
                                                                                            "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"@id\" : \"MOVE\" , \"duration\" : %4 , \"@next\" : \"EMPTY_WASTE\" } , "
                                                                                            "{ \"@type\" : \"ExchangePropertyBehaviour\" ,   \"@id\" : \"EMPTY_WASTE\" , \"duration\" : 1000 , \"@next\" : \"SET_COLOR_EMPTY\" }  , "
                                                                                            "{ \"@type\" : \"SetAgentPropertyBehaviour\" , \"@id\" : \"SET_COLOR_EMPTY\" , \"property_name\" : \"color\" , \"property_value\" : \"Green\" , \"duration\" : 1000 , \"@next\" : \"SET_HOME_X\" } , "
                                                                                            "{ \"@type\" : \"SetSkillPropertyBehaviour\" , \"@id\" : \"SET_HOME_X\" , \"skill\" : \"MoveThroughRouteSkill\" , \"skill_property_name\" : \"route_destination_x\" , \"skill_property_value\" : %1 , \"duration\" : 1000 , \"@next\" : \"SET_HOME_Y\" } , "
                                                                                            "{ \"@type\" : \"SetSkillPropertyBehaviour\" , \"@id\" : \"SET_HOME_Y\" , \"skill\" : \"MoveThroughRouteSkill\" , \"skill_property_name\" : \"route_destination_y\" , \"skill_property_value\" : %2 , \"duration\" : 1000 , \"@next\" : \"MOVE_HOME\" }  , "
                                                                                            "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"@id\" : \"MOVE_HOME\" , \"duration\" : %4 , \"@next\" : \"INCREMENT\" }  "
                                                                                            " ] } ")
                                                       .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution()  + lon_min )
                                                       .arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )
                                                       .arg( qrand() % 25 + 1 )
                                                        .arg( (qrand() % 4 + 1) * 250 )
                                                       .toLatin1()
                                                        );



        QSharedPointer<GWSAgent> human = GWSObjectFactory::globalInstance()->fromJSON( jsonHumans.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( human );
        emit GWSApp::globalInstance()->sendAgentSignal( human ->serialize() );

    }*/

    QJsonDocument json = QJsonDocument::fromJson(
                                            QString( "{ \"@type\" : \"GWSAgent\" , \"@family\": [ \"GWSAgent\" ], \"color\" : \"red\" , "
                                                     "\"@behaviours\": [ { \"@id\": \"BH0\", \"@type\": \"CheckIfAtPositionBehaviour\", \"duration\": 1, \"position_x_value\": -2.45423, \"position_y_value\": 43.3423, \"nexts_if_not_at_position\": [ \"BH0\" ], \"nexts_if_at_position\": [ \"BH1\" ] } , "
                                                                        "{ \"@type\": \"GenerateWasteBehaviour\", \"@id\": \"BH1\", \"duration\": 1, \"waste_type\": \"batteries\" } "
                                                     "] }" )
                                            .toLatin1() );
    QSharedPointer<GWSAgent> element = GWSObjectFactory::globalInstance()->fromJSON( json.object() ).dynamicCast<GWSAgent>();
    qDebug() << element->serialize();
    /* ----------------
     * Truck Agents
     * ----------------*/

    for( int i = 0 ; i < 0 ; i++ ){

        QJsonDocument jsonTrucks = QJsonDocument::fromJson( QString("{ \"@type\" : \"TruckAgent\" , "
                                                                     "\"waste_amount\" : 0 , "
                                                                     "\"home_coordX\" :  %1, "
                                                                     "\"home_coordY\" :  %2, "
                                                                     "\"color\" : \"Blue\" , "
                                                                     "\"loop_stage\"  : 0 , "
                                                                     "\"wait_for_me\" : true , "
                                                                     "\"@skills\" : [ { \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 40 } , "
                                                                                     "{ \"@type\" : \"GenerateOrderedTSPSkill\" , \"visit_agents_type\" : \"ContainerAgent\" }  ]  , "
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0] } , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/226/226592.svg\" , \"color\" : \"purple\" } , "
                                                                    "\"@behaviours\" : [   { \"@type\" : \"MoveAlongTSPRouteBehaviour\" , \"start\" : true ,  \"@id\" : \"LOOP_STAGES\" , \"duration\" : 1000 , \"@next\" : \"MOVE_STAGES\" } , "
                                                                                          "{ \"@type\" : \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_STAGES\" , \"duration\" : 1000 , \"@next\" : [\"EXCHANGE_WASTE\"] } , "
                                                                                          "{ \"@type\" : \"ExchangePropertyBehaviour\" ,   \"@id\" : \"EXCHANGE_WASTE\" , \"duration\" : 1000 , \"@next\" : [\"SET_COLOR_EXCHANGED\"]  } ,"
                                                                                          "{ \"@type\" : \"SetAgentPropertyBehaviour\" , \"@id\" : \"SET_COLOR_EXCHANGED\" , \"property_name\" : \"color\" , \"property_value\" : \"Orange\" , \"duration\" : 1000 , \"@next\" : \"LOOP_STAGES\" }  "
                                                                                          " ] } ")
                                                       .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution() + lon_min )
                                                       .arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )
                                                       .toLatin1()
                                                        );

        QSharedPointer<GWSAgent> trucks = GWSObjectFactory::globalInstance()->fromJSON( jsonTrucks.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( trucks );
        emit GWSApp::globalInstance()->sendAgentSignal( trucks ->serialize() );

    }

    /* ---------------------------
     * Zamudio census
     * ---------------------------*/

    // Generated from a CSV with headers: x, y, total, varones, mujeres, direccion

    // Read container data from datasource url:
    GWSDatasourceReader* censusReader = new GWSDatasourceReader( "https://datasources.geoworldsim.com/api/datasource/f8054929-c791-4777-b4bf-954dbf05166a/read" );

    censusReader->connect( censusReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){


        QJsonDocument jsonHumans = QJsonDocument::fromJson( QString("{ \"@type\" : \"HumanAgent\" , "
                                                                    "\"waste_amount\" : 0 , "
                                                                    "\"home_coordX\" : %1 , "
                                                                    "\"home_coordY\" : %2 , "
                                                                    "\"color\" : \"Green\" , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 25 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/145/145852.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                            "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"start\" : true , \"@id\" : \"INCREMENT\" ,  \"property\" : \"waste_amount\" , \"increment\" : %3 , \"max\" : 100. , \"min\" : -1 , \"duration\" : 1000 , \"@next\" : \"SET_COLOR_FULL\" } , "
                                                                                            "{ \"@type\" : \"SetAgentPropertyBehaviour\" , \"@id\" : \"SET_COLOR_FULL\" , \"property_name\" : \"color\" , \"property_value\" : \"Red\" , \"duration\" : 1000 , \"@next\" : \"FIND_CLOSEST_CONTAINER\" } , "
                                                                                            "{ \"@type\" : \"FindClosestBehaviour\" , \"@id\" : \"FIND_CLOSEST_CONTAINER\" , \"duration\" : 1000 , \"@next\" : \"MOVE\" } , "
                                                                                            "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"@id\" : \"MOVE\" , \"duration\" : %4 , \"@next\" : \"EMPTY_WASTE\" } , "
                                                                                            "{ \"@type\" : \"ExchangePropertyBehaviour\" ,   \"@id\" : \"EMPTY_WASTE\" , \"duration\" : 1000 , \"@next\" : \"SET_COLOR_EMPTY\" }  , "
                                                                                            "{ \"@type\" : \"SetAgentPropertyBehaviour\" , \"@id\" : \"SET_COLOR_EMPTY\" , \"property_name\" : \"color\" , \"property_value\" : \"Green\" , \"duration\" : 1000 , \"@next\" : \"SET_HOME_X\" } , "
                                                                                            "{ \"@type\" : \"SetSkillPropertyBehaviour\" , \"@id\" : \"SET_HOME_X\" , \"skill\" : \"MoveThroughRouteSkill\" , \"skill_property_name\" : \"route_destination_x\" , \"skill_property_value\" : %1 , \"duration\" : 1000 , \"@next\" : \"SET_HOME_Y\" } , "
                                                                                            "{ \"@type\" : \"SetSkillPropertyBehaviour\" , \"@id\" : \"SET_HOME_Y\" , \"skill\" : \"MoveThroughRouteSkill\" , \"skill_property_name\" : \"route_destination_y\" , \"skill_property_value\" : %2 , \"duration\" : 1000 , \"@next\" : \"MOVE_HOME\" }  , "
                                                                                            "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"@id\" : \"MOVE_HOME\" , \"duration\" : %4 , \"@next\" : \"INCREMENT\" }  "
                                                                                            " ] } ")
                                                       .arg( data.value( "X" ).toString() )
                                                       .arg( data.value( "Y" ).toString() )
                                                       .arg( qrand() % 25 + 1 )
                                                       .arg( (qrand() % 4 + 1) * 250 )
                                                       .toLatin1()
                                                        );

        QSharedPointer<GWSAgent> humans = GWSObjectFactory::globalInstance()->fromJSON( jsonHumans.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( humans );
        emit GWSApp::globalInstance()->sendAgentSignal( humans->serialize() );

    } );

    //censusReader->startReading();






    /* ----------------
     * Container Agents
     * ----------------*/

    // Read container data from datasource url:
    GWSDatasourceReader* containerReader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/efd5cf54-d737-4866-9ff3-c82d129ea44b/read" );

    containerReader->connect( containerReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

            // Create GEOMETRY JSON
            QJsonObject geo = data.value("geometry").toObject();
            geo.insert( "@type" , "GWSGeometry" );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
            //agent_json.insert( "node" , node );
            agent_json.insert( "@type" , "ContainerAgent" );
            agent_json.insert( "id" , data.value( "osm_id" ) );
            agent_json.insert( "waste_amount" , 0 );  // This is the attribute that should be changed when humans throw their waste.

            QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            container->setProperty( GWSAgent::STYLE_ICON_URL_PROP , "https://image.flaticon.com/icons/svg/382/382314.svg" );

            emit GWSApp::globalInstance()->sendAgentSignal( container->serialize() );

    } );

    containerReader->startReading();


    /* ----------------
     * Zamudio roads
     * ----------------*/


    // Read PEDESTRIAN ROAD data from datasource url:
    GWSDatasourceReader* pedestrian_reader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/0204533e-104e-4878-ac56-79c8960da545/read" );

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
            agent_json.insert( "geo" , geo);
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentSignal( pedestrian->serialize() );
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
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentSignal( pedestrian->serialize() );
        }

        }
        catch (std::exception &e){

        }

    });

    // Read MISSING ROAD data from datasource url:
    GWSDatasourceReader* other_reader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/92101b3d-faf6-46a0-97b6-ef48747c07ef/read" );

    other_reader->connect( other_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
            agent_json.insert( "geo" , geo);
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> other = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentSignal( other->serialize() );
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
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> other = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

           // GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
           // env->getId();

            emit GWSApp::globalInstance()->sendAgentSignal( other->serialize() );
        }

        }
        catch (std::exception &e){

        }
    });


    // Read FOOTWAY ROAD data from datasource url:
    GWSDatasourceReader* footway_reader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/5623ec42-56a3-46b2-afd6-27a74613bbde/read" );

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
            agent_json.insert( "geo" , geo);
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentSignal( footway->serialize() );
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
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            //QJsonArray family_array; family_array.append( "Road" );
            //agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            //GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            //env->getId();

            emit GWSApp::globalInstance()->sendAgentSignal( footway->serialize() );
        }

        }
        catch (std::exception &e){

        }
    });


    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( HumanAgent::staticMetaObject.className() ) ){
            GWSExecutionEnvironment::globalInstance()->registerAgent( a );
        }

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( TruckAgent::staticMetaObject.className() ) ){
            GWSExecutionEnvironment::globalInstance()->registerAgent( a );
        }

    });

        footway_reader->startReading();
        pedestrian_reader->startReading();
        //other_reader->startReading();


        QTimer::singleShot( 10*1000 , [](){
            GWSExecutionEnvironment::globalInstance()->run();
        } );
    app->exec();

}


