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
#include "../../behaviour/waste4think/GenerateAgentGeometryBehaviour.h"
#include "../../behaviour/check/CheckIfAtPositionBehaviour.h"
#include "../../behaviour/waste4think/GenerateWasteBehaviour.h"
#include "../../behaviour/waste4think/FindClosestBehaviour.h"
#include "../../behaviour/waste4think/TransferAgentPropertyBehaviour.h"
#include "../../behaviour/move/MoveBehaviour.h"

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
#include "../../util/datasource/AgentGeneratorDatasource.h"
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
    /*GWSObjectFactory::globalInstance()->registerType( ViewSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveSkill::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveThroughRouteSkill::staticMetaObject );*/

    // Behaviours
    GWSObjectFactory::globalInstance()->registerType( GenerateAgentGeometryBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( CheckIfAtPositionBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( GenerateWasteBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( MoveBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( FindClosestBehaviour::staticMetaObject );
    GWSObjectFactory::globalInstance()->registerType( TransferAgentPropertyBehaviour::staticMetaObject );
    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


QJsonDocument agent_json = QJsonDocument::fromJson( QString( "{ \"@type\": \"GWSAgent\", "
                                                        "\"@family\": [ \"GWSAgent\", \"Citizen\" ], "
                                                        "\"home_x\": -2, \"home_y\": 43, \"waste\": 100 , "
                                                        "\"@behaviours\": [  { \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"start\": true, \"nexts\" : [\"MOVE\"] }, "
                                                                    "{ \"@type\": \"MoveBehaviour\", \"@id\": \"MOVE\", \"duration\": 1, \"maxspeed\": 400, \"x_value\": \"<X>\", \"y_value\": \"<Y>\", \"nexts_if_arrived\" : [\"FIND\"], \"nexts_if_not_arrived\" : [\"MOVE\"] }, "
                                                                    "{ \"@type\": \"FindClosestBehaviour\", \"duration\": 1, \"@id\": \"FIND\", \"closest_agent_type\": \"ContainerAgent\", \"transport_network_type\": \"Road\", \"store_closest_id_as\": \"closest_container_id\", \"store_closest_distance_as\": \"closest_container_distance\", \"@id\": \"FIND\", \"nexts\": [ \"TRANSFER\" ] }, "
                                                                    "{ \"@type\": \"TransferAgentPropertyBehaviour\", \"duration\": 1, \"@id\": \"TRANSFER\", \"property_to_transfer\": \"waste\", \"receiving_agent_id\": \"<closest_container_id>\" } "
                                                        "] } ")
                                                        .toLatin1()
                                                        );


AgentGeneratorDatasource* ds = new AgentGeneratorDatasource( agent_json.object() , "https://datasources.geoworldsim.com/api/datasource/f8054929-c791-4777-b4bf-954dbf05166a/read" );

//QSharedPointer<GWSAgent> element = GWSObjectFactory::globalInstance()->fromJSON( agent_json.object() ).dynamicCast<GWSAgent>();
//qDebug() << element->serialize();
//GWSExecutionEnvironment::globalInstance()->registerAgent( element );


/* ----------------
 * Container Agents
 * ----------------*/

// Read container data from datasource url:

GWSDatasourceReader* containerReader = new GWSDatasourceReader( "http://laika.energia.deusto.es:8050/api/datasource/efd5cf54-d737-4866-9ff3-c82d129ea44b/read" );

containerReader->connect( containerReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

        // Create GEOMETRY JSON
        QJsonObject geo = data.value("geometry").toObject();
        geo.insert( "@type" , "GWSGeometry" );

        QJsonObject container_json;
        container_json.insert( "geo" , geo );  // Attribute that should be checked so that the human finds the closest container.
        //agent_json.insert( "node" , node );
        container_json.insert( "@type" , "ContainerAgent" );
        container_json.insert( "id" , data.value( "osm_id" ) );
        container_json.insert( "waste_amount" , 0 );  // This is the attribute that should be changed when humans throw their waste.

        QSharedPointer<GWSAgent> container = GWSObjectFactory::globalInstance()->fromJSON( container_json ).dynamicCast<GWSAgent>();
        container->setProperty( GWSAgent::STYLE_ICON_URL_PROP , "https://image.flaticon.com/icons/svg/382/382314.svg" );

        emit GWSApp::globalInstance()->sendAgentSignal( container->serialize() );

        //qDebug() << GWSAgentEnvironment::globalInstance()->getByClass( "ContainerAgent" ).size();

       // qDebug() << GWSExecutionEnvironment::globalInstance()->getByClass( "ContainerAgent" ).size();


} );

qDebug() << GWSAgentEnvironment::globalInstance()->getByClass( "GWSAgent" ).size();
qDebug() << GWSAgentEnvironment::globalInstance()->getByClass( "ContainerAgent" ).size();

//containerReader->startReading();


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

   // foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( HumanAgent::staticMetaObject.className() ) ){
     //   GWSExecutionEnvironment::globalInstance()->registerAgent( a );
    //}

    foreach ( QSharedPointer<GWSAgent> a, GWSAgentEnvironment::globalInstance()->getByClass( TruckAgent::staticMetaObject.className() ) ){
        GWSExecutionEnvironment::globalInstance()->registerAgent( a );
    }

});

    //footway_reader->startReading();
    //pedestrian_reader->startReading();
    //other_reader->startReading();


    QTimer::singleShot( 10*1000 , [](){
        GWSExecutionEnvironment::globalInstance()->run();
    } );
app->exec();

}
