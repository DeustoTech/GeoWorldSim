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


    // Init random numbers
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );


/* --------------
 *  Agents
 *---------------*/

/* Returns a random double between min and max
 Zamudio latitude = 43.2803457
 Zamudio longitude = -2.8621286 */
double lat_max = 43.2685;
double lat_min = 43.2567;
double lon_max = -2.9158;
double lon_min = -2.9503;

for( int i = 0 ; i < 1 ; i++ ){

    QJsonDocument jsonAgent = QJsonDocument::fromJson( QString("{ \"@type\" : \"GWSAgent\" , \"running\" : true, "
                                                                  "\"home_x\" :  %1, \"home_y\" :  %2,  \"color\" : \"Blue\" , "
                                                                  "\"@behaviours\" : [  { \"@type\": \"SendAgentSnapshotBehaviour\" ,   \"@id\": \"HISTORY\" , \"duration\": 1 , \"start\": true, \"nexts\" : [\"GEOM\"] } ,"
                                                                                       "{ \"@type\": \"GenerateAgentGeometryBehaviour\", \"@id\": \"GEOM\", \"duration\": 1 , \"x_value\": %1,  \"y_value\": %2, \"nexts\" : [\"MOVE\"] }, "
                                                                                       "{ \"@type\": \"MoveThroughRouteBehaviour\" ,   \"@id\" : \"MOVE\" , \"duration\" : 1 , \"maxspeed\" : 150 , \"x_value\": -2.9314 , \"y_value\": 43.2644 , \"store_total_moved_distance_as\" : \"total_moved_distance\" , \"store_total_travel_time_as\" : \"total_travel_time\" ,  \"nexts_if_arrived\" : [\"HISTORY\"] , \"nexts_if_not_arrived\" : [\"MOVE\"] }  "
                                                                                       " ] } ")
                                                   .arg( (lon_max - lon_min) * UniformDistribution::uniformDistribution()  + lon_min )
                                                   .arg( (lat_max - lat_min) * UniformDistribution::uniformDistribution() + lat_min )
                                                   .toLatin1()
                                                    );

    QSharedPointer<GWSAgent> agent = GWSObjectFactory::globalInstance()->fromJSON( jsonAgent.object() ).dynamicCast<GWSAgent>();


}

/* ----------------
 * Bilbao roads
 * ----------------*/

// Read Primary Road data from datasource url:
GWSDatasourceReader* primaryHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/92afa3e7-62fa-4ec3-803b-c60e93e63dcd/read" );

primaryHighwayReader->connect( primaryHighwayReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> primary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( primary->serialize() );
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

            QSharedPointer<GWSAgent> primary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( primary->serialize() );
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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> primary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( primary->serialize() );

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();
        }
        }
        catch (std::exception &e){
        }

    }

});



// Read Secondary Road data from datasource url:
GWSDatasourceReader* secondaryHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/a1c2a08b-9e16-4fdf-86d2-84c611c325fc/read" );


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
            QJsonArray family_array; family_array.append( "Road" );
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
            QJsonArray family_array; family_array.append( "Road" );
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
            QJsonArray family_array; family_array.append( "Road" );
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

});



// Read Tertiary Road data from datasource url:
GWSDatasourceReader* tertiaryHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/f2e32381-f035-49d9-8836-195ed020a26d/read" );

tertiaryHighwayReader->connect( tertiaryHighwayReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> tertiary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( tertiary->serialize() );
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

            QSharedPointer<GWSAgent> tertiary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( tertiary->serialize() );
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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> tertiary = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( tertiary->serialize() );

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();
        }
        }
        catch (std::exception &e){
        }

    }

});


// Read Trunk Higway data from datasource url:
GWSDatasourceReader* trunkHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/a0d53dac-d0da-496e-9231-2cf526992429/read" );

trunkHighwayReader->connect( trunkHighwayReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> trunk = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( trunk->serialize() );
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

            QSharedPointer<GWSAgent> trunk = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( trunk->serialize() );
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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> trunk = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( trunk->serialize() );

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();
        }
        }
        catch (std::exception &e){
        }

    }

});

// Read Residential Road data from datasource url:
GWSDatasourceReader* residentialHighwayReader = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/ecd490dd-a01d-4eb7-b471-01c90a281cb7/read" );

residentialHighwayReader->connect( residentialHighwayReader , &GWSDatasourceReader::dataValueReadSignal , []( QJsonObject data ){

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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> residential = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( residential->serialize() );
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

            QSharedPointer<GWSAgent> residential = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( residential->serialize() );
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
            QJsonArray family_array; family_array.append( "Road" );
            agent_json.insert( "@family" , family_array );
            agent_json.insert( "@type" , "GWSAgent" );

            QSharedPointer<GWSAgent> residential = GWSObjectFactory::globalInstance()->fromJSON( agent_json ).dynamicCast<GWSAgent>();

            emit GWSApp::globalInstance()->sendAgentToSocketSignal( residential->serialize() );

            GWSNetworkEnvironment* env = GWSNetworkEnvironment::globalInstance();
            env->getId();
        }
        }
        catch (std::exception &e){
        }

    }

});




QTimer::singleShot( 100 , [primaryHighwayReader](){
    primaryHighwayReader->startReading();
});

QTimer::singleShot( 100 , [secondaryHighwayReader](){
    secondaryHighwayReader->startReading();
});

QTimer::singleShot( 100 , [tertiaryHighwayReader](){
    tertiaryHighwayReader->startReading();
});

QTimer::singleShot( 100 , [trunkHighwayReader](){
    trunkHighwayReader->startReading();
});

QTimer::singleShot( 10000 , [residentialHighwayReader](){
    residentialHighwayReader->startReading();
});



QTimer::singleShot( 10*1000 , [](){

    GWSTimeEnvironment::globalInstance()->setDatetime( 1000 );

   /* foreach (QSharedPointer<GWSAgent> a , GWSAgentEnvironment::globalInstance()->getByClass( ContainerAgent::staticMetaObject.className() ) ) {
        a->setProperty( "color" , QColor::colorNames().at( qrand() % QColor::colorNames().size() ) );
    }*/

    GWSExecutionEnvironment::globalInstance()->run();
} );


app->exec();

}
