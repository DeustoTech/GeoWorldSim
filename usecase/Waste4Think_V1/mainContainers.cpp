#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>
#include <QtMath>


#include "TesisBehaviour.h"
#include "../../agent/Agent.h"

// Agents
#include "ContainerAgent.h"
#include "HumanAgent.h"
#include "TruckAgent.h"


// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
//#include "../../skill/move/MoveAlongStagedRouteSkill.h"

// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/property/IncrementPropertyBehaviour.h"
#include "../../behaviour/waste/EmptyWasteBehaviour.h"
#include "../../behaviour/move/SelectDestinationBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/FindClosestBehaviour.h"
#include "../../behaviour/move/GoHomeBehaviour.h"
#include "../../behaviour/check/CheckIfAtPositionBehaviour.h"
#include "../../behaviour/check/CheckIfPropertyBehaviour.h"
#include "../../behaviour/check/CheckIfAtOtherAgentsPositionBehaviour.h"
//#include "../../behaviour/move/MoveAlongStagedRouteBehaviour.h"
#include "../../behaviour/move/LoopOverRouteStagesBehaviour.h"
#include "../../behaviour/property/ExchangePropertyBehaviour.h"
#include "../../behaviour/information/BroadcastToHistoryBehaviour.h"
#include "../../behaviour/move/FindClosestEdgePointBehaviour.h"

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
    //GWSObjectFactory::globalInstance()->registerType( MoveAlongStagedRouteSkill::staticMetaObject );

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    //GWSObjectFactory::globalInstance()->registerType( MoveAlongStagedRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( EmptyWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GoHomeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtOtherAgentsPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( LoopOverRouteStagesBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( ExchangePropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( BroadcastToHistoryBehaviour::staticMetaObject);
    GWSObjectFactory::globalInstance()->registerType( FindClosestEdgePointBehaviour::staticMetaObject );


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* ----------------
     * Human Agents
     * ----------------*/

    /* Returns a random double between min and max
     Zamudio latitude = 43.2803457
     Zamudio longitude = -2.8621286*/
    double lat_max = 43.28472587082224;
    double lat_min = 43.280961278501344;
    double lon_max = -2.859949952301804 ;
    double lon_min = -2.8665803729866184;


    // The random position generator will eventually be substituted by data from the census, similar to the procedure for containers

    for( int i = 0 ; i < 0; i++ ){

        QJsonDocument jsonHumans = QJsonDocument::fromJson( QString("{ \"@type\" : \"HumanAgent\" , "
                                                                    "\"waste_amount\" : 0 , "
                                                                    "\"home_coordX\" : %1 , "
                                                                    "\"home_coordY\" : %2 , "
                                                                     "\"wait_for_me\" : true , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 8 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/145/145852.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                            "{ \"@type\" : \"CheckIfAtPositionBehaviour\", \"start\" : true , \"duration\" : 1000 , \"key_position_x\" : %1 , \"key_position_y\" : %2 , \"@next\" : [\"INCREMENT\",\"WASTE_FULL\"] } , "
                                                                                            "{ \"@type\" : \"CheckIfPropertyBehaviour\", \"@id\" : \"WASTE_FULL\" , \"duration\" : 1000 , \"property_name\" : \"waste_amount\" , \"check_value\" : 100 , \"@next\" : [\"FIND_CLOSEST\"] } , "
                                                                                            "{ \"@type\" : \"CheckIfAtOtherAgentsPositionBehaviour\", \"start\" : true , \"duration\" : 1000 , \"@next\" : [\"EMPTY_WASTE\"] } , "
                                                                                            "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"start\" : true , \"duration\" : 1000 } , "
                                                                                            "{ \"@type\" : \"BroadcastToHistoryBehaviour\" , \"start\" : true ,  \"duration\" : 1000 } , "
                                                                                            "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"@id\" : \"INCREMENT\" ,  \"property\" : \"waste_amount\" , \"increment\" : %3 , \"max\" : 100. , \"min\" : 0 , \"duration\" : 1000  } , "
                                                                                            "{ \"@type\" : \"FindClosestBehaviour\" , \"@id\" : \"FIND_CLOSEST\" , \"duration\" : 1000  } , "
                                                                                            "{ \"@type\" : \"EmptyWasteBehaviour\", \"@id\" : \"EMPTY_WASTE\" , \"duration\" : 1000 , \"@next\" : \"GO_HOME\" } , "
                                                                                            "{ \"@type\" : \"GoHomeBehaviour\" , \"@id\" : \"GO_HOME\" , \"duration\" : 1000  } "
                                                                                            " ] } ")
                                                       .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution()  + lon_min )
                                                       .arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )
                                                       .arg( qrand() % 100 + 1 )
                                                       .toLatin1()
                                                        );

        QSharedPointer<GWSAgent> human = GWSObjectFactory::globalInstance()->fromJSON( jsonHumans.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( human );
        emit GWSApp::globalInstance()->sendAgentSignal( human ->serialize() );
        qDebug() << human ->serialize();
    }


    /* ----------------
     * Truck Agents
     * ----------------*/

    for( int i = 0 ; i < 1 ; i++ ){

        QJsonDocument jsonTrucks = QJsonDocument::fromJson( QString("{ \"@type\" : \"TruckAgent\" , "
                                                                     "\"waste_amount\" : 0 , "
                                                                     "\"home_coordX\" :  %1, "
                                                                     "\"home_coordY\" :  %2, "
                                                                     "\"loop_stage\"  : 0 , "
                                                                     "\"wait_for_me\" : true , "
                                                                     "\"@skills\" : [ { \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 10 } ]  , "
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0] } , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/226/226592.svg\" , \"color\" : \"purple\" } , "
                                                                    "\"@behaviours\" : [   { \"@type\" : \"LoopOverRouteStagesBehaviour\" , \"start\" : true ,  \"@id\" : \"LOOP_STAGES\" , \"duration\" : 1000 , \"@next\" : \"MOVE_STAGES\" } , "
                                                                                          "{ \"@type\" : \"FindClosestEdgePointBehaviour\", \"start\" : true , \"duration\" : 1000 } , "
                                                                                          "{ \"@type\" : \"BroadcastToHistoryBehaviour\" , \"start\" : true ,  \"duration\" : 1000 } , "
                                                                                          "{ \"@type\" : \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE_STAGES\" , \"duration\" : 1000 } , "
                                                                                          "{ \"@type\" : \"CheckIfAtOtherAgentsPositionBehaviour\", \"start\" : true , \"duration\" : 1000 , \"@next\" : [\"EXCHANGE_WASTE\"] } , "
                                                                                          "{ \"@type\" : \"ExchangePropertyBehaviour\" ,   \"@id\" : \"EXCHANGE_WASTE\" , \"duration\" : 1000  } "
                                                                    " ] } ")
                                                       .arg( -2.8644994224613356  )
                                                       .arg( 43.28428545332489 )
                                                       .toLatin1()
                                                        );

        // .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution()  + lon_min )
        //.arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )

        QSharedPointer<GWSAgent> trucks = GWSObjectFactory::globalInstance()->fromJSON( jsonTrucks.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( trucks );

        emit GWSApp::globalInstance()->sendAgentSignal( trucks ->serialize() );

    }



    /* ----------------
     * Container Agents
     * ----------------*/

    // Read container data from datasource url:
    GWSDatasourceReader* containerReader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/efd5cf54-d737-4866-9ff3-c82d129ea44b/read" );

    containerReader->connect( containerReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

            // Create GEOMETRY JSON
            QJsonObject geo = data.value("geometry").toObject();
            geo.insert( "@type" , "GWSGeometry" );

            // Create GRAPHNODE JSON
            /*QJsonObject node;
            node.insert( "@type" , "GWSGraphNode" );
            node.insert( "node_x" , geo.value( "coordinates" ).toArray().at( 0 ) );
            node.insert( "node_y" , geo.value( "coordinates" ).toArray().at( 1 ) );*/

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
            //agent_json.insert( "node" , node );
            agent_json.insert( "@type" , "ContainerAgent" );
            agent_json.insert( "id" , data.value( "osm_id" ) );
            agent_json.insert( "waste_amount" , 0 );  // This is the attribute that should be changed when humans throw their waste.

            QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

            emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );


    } );

    containerReader->startReading();

    // Create 4 containers
    /*{
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.8642509898002118 );
        coors.append( 43.28394217429335 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "@id" , "CONTAINER1" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }

    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.863631059684394  );
        coors.append( 43.28349894289374 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "@id" , "CONTAINER2" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }

    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.863246887189689 );
        coors.append( 43.282988768169616 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "@id" , "CONTAINER3" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }

    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.8636843417772297 );
        coors.append( 43.282377094765025 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "@id" , "CONTAINER4" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }*/


    /*{
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.862962516695802 );
        coors.append( 43.28158014470859 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "id" , "CONTAINER5" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }

    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.8616804207866835 );
        coors.append( 43.28318909049451 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "id" , "CONTAINER6" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container->serialize() );
    }


    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.863166364580934 );
        coors.append( 43.28387053650099 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "id" , "CONTAINER7" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }


    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.8638798321788954  );
        coors.append( 43.28185448859504 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "id" , "CONTAINER8" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }

    {
        QJsonObject geo;
        geo.insert( "@type" , "GWSGeometry" );
        geo.insert( "type" , "Point" );
        QJsonArray coors;
        coors.append( -2.862347847161118  );
        coors.append( 43.282492136677476 );
        geo.insert( "coordinates" , coors );

        QJsonObject agent_json;
        agent_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        agent_json.insert( "@type" , "ContainerAgent" );
        agent_json.insert( "id" , "CONTAINER8" );
        agent_json.insert( "waste_amount" , 0 );

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
        container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

        emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );
    }*/




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

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

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

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

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

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className() ) ){
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


