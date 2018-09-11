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

// Skills
#include "../../skill/view/ViewSkill.h"
#include "../../skill/move/MoveSkill.h"
#include "../../skill/move/MoveThroughRouteSkill.h"
// Behaviours
#include "../../behaviour/Behaviour.h"
#include "../../behaviour/property/IncrementPropertyBehaviour.h"
#include "../../behaviour/waste/DecideAccordingToWasteBehaviour.h"
#include "../../behaviour/waste/EmptyWasteBehaviour.h"
#include "../../behaviour/move/SetHomeBehaviour.h"
#include "../../behaviour/move/SelectDestinationBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/FindClosestBehaviour.h"
#include "../../behaviour/move/GoHomeBehaviour.h"
#include "../../behaviour/move/CheckIfAtPositionBehaviour.h"

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

    // Skills
    GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteSkill::staticMetaObject );


    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( DecideAccordingToWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( IncrementPropertyBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( EmptyWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( SetHomeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GoHomeBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );

    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* ----------------
     * Human Agents
     * ----------------*/

    /* Returns a random double between min and max
     Zamudio latitude = 43.2803457
     Zamudio longitude = -2.8621286*/
    double lat_max = 43.28139;
    double lat_min = 43.27554;
    double lon_max = -2.84024;
    double lon_min = -2.87092;


    // The random position generator will eventually be substituted by data from the census, similar to the procedure for containers

    for( int i = 0 ; i < 500 ; i++ ){

        QJsonDocument jsonHumans = QJsonDocument::fromJson( QString("{ \"@type\" : \"HumanAgent\" , "
                                                                     "\"waste_amount\" : 0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 25 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/145/145852.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
                                                                                           "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"@id\" : \"BH6\" , \"duration\" : 1000 } ,  "
                                                                                           "{ \"@type\" : \"GoHomeBehaviour\" , \"@id\" : \"BH5\" , \"@next\" : \"BH6\" , \"duration\" : 1000  } , "
                                                                                           "{ \"@type\" : \"EmptyWasteBehaviour\", \"@id\" : \"BH4\" , \"@next\" : \"BH5\" , \"duration\" : 1000 } , "
                                                                                           "{ \"@type\" : \"MoveThroughRouteBehaviour\", \"@id\" : \"BH3\" , \"@next\" : \"BH4\" , \"duration\" : 1000 } , "
                                                                                           "{ \"@type\" : \"FindClosestBehaviour\" , \"@id\" : \"BH2\" , \"@next\" : \"BH3\" , \"duration\" : 1000  } , "
                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"@id\" : \"BH1\" ,  \"property\" : \"waste_amount\" , \"increment\" : %3 , \"max\" : 100. , \"min\" : 0 , \"duration\" : 1000  } , "
                                                                                           "{ \"@type\" : \"CheckIfAtPositionBehaviour\" , \"@id\" : \"BH7\" , \"@next\" : \"BH1\" , \"key_position_x\" : %1 , \"key_position_y\" : %2 , \"duration\" : 1000  } , "
                                                                                           "{ \"@type\" : \"DecideAccordingToWasteBehaviour\" , \"@id\" : \"BH0\" , \"duration\" : 1000 } , "
                                                                                           "{ \"@type\" : \"SetHomeBehaviour\" , \"duration\" : 1000 , \"start\" : true , \"@next\" : \"BH0\" } "
                                                                                      " ] } ")
                                                       .arg( (lon_max - lon_min) * ( (double)qrand() / (double)RAND_MAX ) + lon_min )
                                                       .arg( (lat_max - lat_min) * ( (double)qrand() / (double)RAND_MAX ) + lat_min )
                                                       .arg( qrand() % 100 + 1 )
                                                       .toLatin1()
                                                        );
         /*"\"@behaviours\" : [  "
                                                                                           "{ \"@type\" : \"DecideAccordingToWasteBehaviour\" , \"@id\" : \"BH0\" , \"duration\" : 1000 , \"next\" : [\"BH1\", \"BH2\"] } , "
                                                                                           "{ \"@type\" : \"IncrementPropertyBehaviour\" , \"@id\" : \"BH1\" , \"property\" : \"waste_amount\" , \"increment\" : %3 , \"max\" : 100. , \"min\" : 0 , \"duration\" : 1000  } , "
                                                                                           "{ \"@type\" : \"GWSBehaviour\" , \"@id\" : \"BH2\" , \"@sub_behaviours\" : ["
                                                                                                                                                                        "{ \"@type\" : \"FindClosestBehaviour\" , \"duration\" : 1000  } , "
                                                                                                                                                                        "{ \"@type\" : \"MoveThroughRouteBehaviour\", \"duration\" : 1000 } , "
                                                                                                                                                                        "{ \"@type\" : \"EmptyWasteBehaviour\", \"duration\" : 1000 } , "
                                                                                                                                                                        "{ \"@type\" : \"GoHomeBehaviour\" , \"duration\" : 1000  } , "
                                                                                                                                                                        "{ \"@type\" : \"MoveThroughRouteBehaviour\" , \"duration\" : 1000 }  "
                                                                                                                                                                        "] } ,"
                                                                                           "{ \"@type\" : \"SetHomeBehaviour\" , \"duration\" : 1000 , \"start\" : true } "
                                                                                      " ]*/

        QSharedPointer<GWSAgent> human = GWSObjectFactory::globalInstance()->fromJSON( jsonHumans.object() ).dynamicCast<GWSAgent>();
        emit GWSApp::globalInstance()->sendAgentSignal( human ->serialize() );

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
            GWSExecutionEnvironment::globalInstance()->registerAgent( container );

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
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( pedestrian );

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
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( pedestrian );

            //emit GWSApp::globalInstance()->pushAgentSignal( pedestrian->serialize() );
        }

        } catch (std::exception &e) {

        }
    });

    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){

        foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( HumanAgent::staticMetaObject.className() ) ){
            GWSExecutionEnvironment::globalInstance()->registerAgent( a );
        }
        GWSExecutionEnvironment::globalInstance()->run();

    });

    footway_reader->startReading();

    app->exec();

    }
