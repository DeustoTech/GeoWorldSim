#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QProcess>


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
#include "../../behaviour/move/TravelingSalesmanProblemBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( EmptyWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GoHomeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtOtherAgentsPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TravelingSalesmanProblemBehaviour::staticMetaObject );

    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* ----------------
     * Human Agents
     * ----------------*/

    /* Returns a random double between min and max
     Zamudio latitude = 43.2803457
     Zamudio longitude = -2.8621286*/
    double lat_max = 43.28539;
    double lat_min = 43.27554;
    double lon_max = -2.85024;
    double lon_min = -2.87092;


    // The random position generator will eventually be substituted by data from the census, similar to the procedure for containers

    for( int i = 0 ; i < 1 ; i++ ){

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
        emit GWSApp::globalInstance()->sendAgentSignal( human ->serialize() );
        qDebug() << human ->serialize();
    }

    for( int i = 0 ; i < 1 ; i++ ){

        QJsonDocument jsonTrucks = QJsonDocument::fromJson( QString("{ \"@type\" : \"TruckAgent\" , "
                                                                     "\"waste_amount\" : 0 , "
                                                                     "\"home_coordX\" : %1 , "
                                                                     "\"home_coordY\" : %2 , "
                                                                     "\"wait_for_me\" : true , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 8 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/776/776588.svg\" , \"color\" : \"purple\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                            "{ \"@type\" : \"TravelingSalesmanProblemBehaviour\"  , \"start\" : true , \"duration\" : 1000  } "
                                                                                            " ] } ")
                                                       .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution()  + lon_min )
                                                       .arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )
                                                       .toLatin1()
                                                        );

        QSharedPointer<GWSAgent> trucks = GWSObjectFactory::globalInstance()->fromJSON( jsonTrucks.object() ).dynamicCast<GWSAgent>();
        emit GWSApp::globalInstance()->sendAgentSignal( trucks ->serialize() );
        qDebug() << trucks ->serialize();
    }



    /* ----------------
     * Container Agents
     * ----------------*/

    // Read container data from datasource url:
    GWSDatasourceReader* reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/b6ec38d9-ce51-48b3-9934-839c4943cee3/read" );

    reader->connect( reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

            // Create GEOMETRY JSON
            QJsonObject geo = data.value("the_geom").toObject();
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
            agent_json.insert( "@id" , data.value( "id" ) );
            agent_json.insert( "capacity" , 100 );  // This is the attribute that should be changed when humans throw their waste.

            QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            container->icon_url = "https://image.flaticon.com/icons/svg/382/382314.svg";

            emit GWSApp::globalInstance()->sendAgentSignal( container ->serialize() );

    } );
    reader->startReading();



    /* ----------------
     * Zamudio roads
     * ----------------*/

    // Read PEDESTRIAN ROAD data from datasource url:
    GWSDatasourceReader* footway_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/22960ed3-59be-443e-8ff7-8b3a5f8d29ac/read" );

    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

        try {
        {
            QJsonObject geo = data.value( "geo").toObject();
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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            //emit GWSApp::globalInstance()->pushAgentSignal( pedestrian->serialize() );

        }
        {
            QJsonObject geo = data.value( "geo").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().last().toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().last().toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            //emit GWSApp::globalInstance()->pushAgentSignal( pedestrian->serialize() );
        }

        } catch (std::exception &e) {

        }
    });

    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( HumanAgent::staticMetaObject.className() ) ){
            GWSExecutionEnvironment::globalInstance()->registerAgent( a );
        }

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( TruckAgent::staticMetaObject.className() ) ){
            GWSExecutionEnvironment::globalInstance()->registerAgent( a );
        }
        GWSExecutionEnvironment::globalInstance()->run();

    });

    footway_reader->startReading();

    app->exec();

    }
