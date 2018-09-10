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
#include "../../behaviour/waste/CheckGeneratedWasteAmountBehaviour.h"
#include "../../behaviour/waste/EmptyWasteBehaviour.h"
#include "../../behaviour/move/SetHomeBehaviour.h"
#include "../../behaviour/move/SelectDestinationBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"
#include "../../behaviour/move/MoveThroughRouteBehaviour.h"
#include "../../behaviour/move/FindClosestBehaviour.h"
#include "../../behaviour/move/GoHomeBehaviour.h"

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
    GWSObjectFactory::globalInstance()->registerType( GoHomeBehaviour::staticMetaObject );

    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


    /* ----------------
     * Human Agents
     * ----------------*/

    /* Returns a random double between min and max
     Zamudio latitude = 43.2803457
     Zamudio longitude = -2.8621286*/
    double lon_max = 43.3;
    double lon_min = 43.2;
    double lat_max = -2.8;
    double lat_min = -2.9;

    // The random position generator will eventually be substituted by data from the census, similar to the procedure for containers

    for( int i = 0 ; i < 1 ; i++ ){

        QJsonDocument jsonHumans = QJsonDocument::fromJson( QString("{ \"@type\" : \"HumanAgent\" , "
                                                                     "\"waste_amount\" : 0 , "
                                                                     "\"@skills\" : [ { \"@type\" : \"ViewSkill\" , \"view_agents_type\" : \"ContainerAgent\" , \"view_geom\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Polygon\" , \"coordinates\" : [[ [-1, -1],[-1, 1],[1, 1],[1, -1],[-1, -1] ]] } } , "
                                                                                     "{ \"@type\" : \"MoveThroughRouteSkill\" , \"maxspeed\" : 800 } ],"
                                                                     "\"geo\" : { \"@type\" : \"GWSGeometry\" , \"type\" : \"Point\" , \"coordinates\" : [ %1 , %2 , 0]} , "
                                                                     "\"style\" : { \"icon_url\" : \"https://image.flaticon.com/icons/svg/145/145852.svg\" , \"color\" : \"red\" } , "
                                                                     "\"@behaviours\" : [  "
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
                                                                                      " ] } ")
                                                       .arg( -2.86101  ) // (lat_max - lat_min) * ( (double)qrand() / (double)RAND_MAX ) + lat_min )
                                                       .arg( 43.28149 ) // (lon_max - lon_min) * ( (double)qrand() / (double)RAND_MAX ) + lon_min )
                                                       .arg( qrand() % 100 + 1 )
                                                       .toLatin1()
                                                        );


        QSharedPointer<GWSAgent> human = GWSObjectFactory::globalInstance()->fromJSON( jsonHumans.object() ).dynamicCast<GWSAgent>();
        GWSExecutionEnvironment::globalInstance()->registerAgent( human );

        emit GWSApp::globalInstance()->pushAgentSignal( human ->serialize() );

    }

    /* ----------------
     * Container Agents
     * ----------------*/

    // Read container data from datasource url:
    GWSDatasourceReader* reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/990ce49d-542f-4ee1-aa3f-309835149e6f/read" );

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

          //  qDebug() << container->serialize();

            emit GWSApp::globalInstance()->pushAgentSignal( container ->serialize() );

    } );
    reader->startReading();


    /* ----------------
     * Zamudio roads
     * ----------------*/

    // Read PEDESTRIAN ROAD data from datasource url:
    GWSDatasourceReader* pedestrian_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/c956ffdd-8460-4d32-969e-600b585f71b6/read" );

    pedestrian_reader->connect( pedestrian_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo);
            agent_json.insert( "edge" , edge );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( pedestrian );

            emit GWSApp::globalInstance()->pushAgentSignal( pedestrian->serialize() );

        }
        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> pedestrian = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( pedestrian );

            emit GWSApp::globalInstance()->pushAgentSignal( pedestrian->serialize() );
        }
    });

    pedestrian_reader->connect( pedestrian_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){

        const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( "GWSAgent" );
        GWSDijkstraRouting* routing = new GWSDijkstraRouting( graph->getEdges() );
        QSharedPointer<GWSGraphEdge> s = graph->findNearestEdge( GWSCoordinate( -2.86453 , 43.28397 ) ); //43.28397, -2.86453
        QSharedPointer<GWSGraphEdge> e = graph->findNearestEdge( GWSCoordinate( -2.86436 , 43.28346 ) ); //43.28346, -2.86436

      //  qDebug() << s->getFrom().toString() << s->getTo().toString();

        QList< QSharedPointer<GWSGraphEdge> > l = routing->dijkstraShortestPath( s->getFrom() , e->getTo() );

       // qDebug() << l;
        foreach( QSharedPointer<GWSGraphEdge> e , l ){
            emit GWSApp::globalInstance()->pushAgentSignal( GWSNetworkEnvironment::globalInstance()->getAgent( e )->serialize() );
        }

    });
    pedestrian_reader->startReading();

    // Read FOOTWAY ROAD data from datasource url:
    GWSDatasourceReader* footway_reader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/683ac1fe-0ad0-4c62-af51-29fd1803acb5/read" );
    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo);
            agent_json.insert( "edge" , edge );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( footway );

            emit GWSApp::globalInstance()->pushAgentSignal( footway->serialize() );
        }
        {
            QJsonObject geo = data.value( "geometry").toObject();
            geo.insert( "@type" ,  "GWSGeometry");

            QJsonObject edge;
            edge.insert( "@type" , "GWSGraphEdge" );
            edge.insert( "edge_to_x" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 0 ) );
            edge.insert( "edge_to_y" , geo.value( "coordinates" ).toArray().at( 0 ).toArray().at( 1 ) );
            edge.insert( "edge_from_x" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 0 ) );
            edge.insert( "edge_from_y" , geo.value( "coordinates" ).toArray().at( geo.value( "coordinates" ).toArray().size() - 1 ).toArray().at( 1 ) );

            QJsonObject agent_json;
            agent_json.insert( "geo" , geo );
            agent_json.insert( "edge" , edge );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> footway = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( footway );

            emit GWSApp::globalInstance()->pushAgentSignal( footway->serialize() );
       }

    });

    footway_reader->connect( footway_reader , &GWSDatasourceReader::dataReadingFinishedSignal , [](){

        const GWSGraph* graph = GWSNetworkEnvironment::globalInstance()->getGraph( "GWSAgent" );
        GWSDijkstraRouting* routing = new GWSDijkstraRouting( graph->getEdges() );
        QSharedPointer<GWSGraphEdge> s = graph->findNearestEdge( GWSCoordinate( -2.86453 , 43.28397 ) );
        QSharedPointer<GWSGraphEdge> e = graph->findNearestEdge( GWSCoordinate( -2.86436 , 43.28346 ) );

      //  qDebug() << s->getFrom().toString() << s->getTo().toString();

        QList< QSharedPointer<GWSGraphEdge> > l = routing->dijkstraShortestPath( s->getFrom() , e->getTo() );

      //  qDebug() << l;
        foreach( QSharedPointer<GWSGraphEdge> e , l ){
            emit GWSApp::globalInstance()->pushAgentSignal( GWSNetworkEnvironment::globalInstance()->getAgent( e )->serialize() );
        }

    });
    footway_reader->startReading();

    GWSExecutionEnvironment::globalInstance()->run();

    app->exec();

    }
