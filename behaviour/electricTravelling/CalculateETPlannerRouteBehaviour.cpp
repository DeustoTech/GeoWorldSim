#include "CalculateETPlannerRouteBehaviour.h"
#include <QJsonDocument>

#include "../../util/polyline_encoder/polylineencoder.h" // https://github.com/vahancho/polylineencoder

QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::ETPLANNER_HOST = "etplanner_host";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::DESTINATION_X = "destination_x";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::DESTINATION_Y = "destination_y";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::DESTINATION_JSON  = "destination_json";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::TRANSPORT_MODE = "transport_mode";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::COLOR = "color";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::TOTAL_DISTANCE = "total_distance";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::ROAD_ID = "road_id";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::IS_STATION = "is_station";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::OPTIMIZATION = "route_optimization";
QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::STOP_ENTITY_IF_NO_ROUTE = "stop_if_no_route";

QString geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::NEXTS = "nexts";


#include "../../util/network/HttpDriver.h"
#include "../../util/geometry/Coordinate.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../skill/move/StoreMultiRouteSkill.h"
#include "../../object/ObjectFactory.h"


geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::CalculateETPlannerRouteBehaviour() : Behaviour(){

}

QPair<double, QJsonArray> geoworldsim::behaviour::CalculateETPlannerRouteBehaviour::behave(){

    QSharedPointer<Entity> agent = this->getEntity();

    QJsonArray next_destinations = agent->getProperty( this->getProperty( skill::StoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ).toString( skill::StoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ) ).toArray();

    // If legs are empty, calculate them through algorithm:
    if ( next_destinations.isEmpty() ){

        geometry::Geometry agent_geom = geometry::Geometry( agent->getProperty( environment::PhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        const geometry::Coordinate& agent_coor = agent_geom.getCentroid();

        double from_x = agent_coor.getX();
        double from_y = agent_coor.getY();

        QJsonValue to_x = this->getProperty( DESTINATION_X );
        QJsonValue to_y = this->getProperty( DESTINATION_Y );


        double dest_y;
        double dest_x;

        if( !to_x.isNull() && !to_y.isNull() ){

            dest_x = to_x.toDouble();
            dest_y = to_y.toDouble();

        }

        QJsonValue json_value = this->getProperty( DESTINATION_JSON );

        if( !json_value.isNull() ){

            geometry::Geometry geom_json = geometry::Geometry( json_value.toObject() );
            const geometry::Coordinate& coor_json = geom_json.getCentroid();

            dest_x = coor_json.getX();
            dest_y = coor_json.getY();
        }

        qint64 currentDateTime = environment::TimeEnvironment::globalInstance()->getCurrentDateTime();
        QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch( currentDateTime );
        QDate date = timeStamp.date();
        QTime time = timeStamp.time();

        // url type to query:
        QString etplanner_host = this->getProperty( "etplanner_host" ).toString( "http://157.158.135.201:8080/etplanner/routers" );
        QString etplanner_pilot = this->getProperty("etplanner_pilot").toString( "bilbao" ).toLower();

        QString etUrl = QString( etplanner_host + "/" + etplanner_pilot + "/plan?fromPlace=%1,%2&toPlace=%3,%4&time=%5&date=%6&mode=%7&chargeLevel=%8&maxWalkDistance=750&maxBikeDistance=10000&maxElectricCarDistance=112654&weightOptimization=%9&requestedResults=1&responseTimeout=3&arriveBy=false&showIntermediateStops=false&energyConsumption=16&energyCost=50&fuelConsumption=8&fuelCost=500&motorFuelConsumption=4&congestionEnabled=false")
                .arg( from_y ).arg( from_x )
                .arg( dest_y ).arg( dest_x )
                .arg( time.toString( "hh:mm" ) )
                .arg( date.toString( "MM-dd-yyyy" ) )
                .arg( "ELECTRIC" )
                .arg( 15 )
                .arg( this->getProperty( OPTIMIZATION ).toString() );

        qDebug() << agent->getProperty("vehicle_subtype");
        qDebug() << etUrl;

        etUrl = "http://157.158.135.201:8080/etplanner/routers/budapest/plan?maxWalkDistance=750&time=12:04&date=2020-07-14&congestionEnabled=false&mode=ELECTRIC&chargeLevel=15&maxBikeDistance=10000&weightOptimization=QUICKER&requestedResults=1&responseTimeout=10&energyCost=50&fuelConsumption=8&fuelCost=50&motorcycleFuelUsage=8&motorcycleFuelCost=50&motivationFrom=&motivationTo=&electricCarIndex=17&minChargeLevel=10&luggageWeight=0&driverWeight=80&arriveBy=false&temperature=23&operatorId=eCar&chargerPlugType=Type2&chargerPhases=1&carWeight=1386&acuCapacity=37&fromPlace=47.46630967143207,19.01656150817871&toPlace=47.501199263815415,19.12033081054688&alreadyRented=false&currentTimestamp=1594721259705";


        agent->incrementBusy(); // IMPORTANT TO WAIT UNTIL REQUEST FINISHES

        network::HttpDriver::globalInstance()->GET( etUrl , [ agent , this ]( QNetworkReply* reply ){

               if( !reply ){
                    agent->decrementBusy();
                    return;
                }

                reply->connect( reply , &QNetworkReply::finished , [ agent , reply , this ] {

                    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();

                    // We are only interested in the "itineraries.legs" field of the QJsonObject

                    if ( !json.isEmpty() ){

                        QJsonObject plan = json.value( "plan" ).toObject();
                        QJsonArray itineraries = plan.value( "itineraries" ).toArray();
                        QJsonObject optimal_itinerary = itineraries.at( 0 ).toObject();
                        QJsonArray legs_array = optimal_itinerary.value( "legs" ).toArray();

                        bool stop_if_no_route = this->getProperty( STOP_ENTITY_IF_NO_ROUTE ).toBool();

                        if( !legs_array.isEmpty() ){

                            // Prepare the next_destinations as required by StoreMultiRouteSkill
                            QSharedPointer<skill::StoreMultiRouteSkill> multiroute_skill = agent->getSkill( skill::StoreMultiRouteSkill::staticMetaObject.className() , true ).dynamicCast<skill::StoreMultiRouteSkill>();
                            if( !multiroute_skill ){
                                multiroute_skill = QSharedPointer<skill::StoreMultiRouteSkill>( new skill::StoreMultiRouteSkill() );
                                agent->addSkill( multiroute_skill );
                            }

                            // Extract array of ordered legs
                            foreach( QJsonValue v , legs_array ){

                                QJsonObject leg = v.toObject();
                                QMap<QString,QString> colors = {{"WALK","Grey"},{"SUBWAY","Yellow"},{"TRAM","Pink"},{"BUS","Blue"},{"MOTORCYCLE","Orange"},{"CAR","Red"},{"ELECTRIC","Lime"}};

                                geometry::Coordinate destination_coor( leg.value( "to" ).toObject().value( "lon" ).toDouble() , leg.value( "to" ).toObject().value( "lat" ).toDouble() );                                

                                // Additional properties
                                QJsonObject properties;
                                properties.insert( TRANSPORT_MODE , leg.value("mode").toString() );
                                //properties.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , leg.value( "startTime" ).toDouble() );
                                properties.insert( COLOR , colors.value( leg.value( "mode" ).toString() , "Black" ) );
                                properties.insert( TOTAL_DISTANCE, leg.value( "distance" ).toDouble() );

                                QJsonArray segments = leg.value( "segments" ).toArray();
                                geometry::Coordinate segment_coor;

                                // Road segments defined by the ETPlanner
                                foreach( QJsonValue s, segments){
                                    QString osm_id = s.toObject().value( "id" ).toString();
                                    QString entity_id = QString("WAY%1").arg( osm_id );
                                    properties.insert( ROAD_ID, entity_id );

                                    auto polyline = gepaf::PolylineEncoder::decode( s.toObject().value( "geometry" ).toObject().value( "points" ).toString().toStdString()  );
                                    geometry::Coordinate segment_coor( polyline.at(polyline.size() - 1 ).longitude(), polyline.at(polyline.size() - 1 ).latitude()  );                                                                       

                                    multiroute_skill->addDestination( segment_coor , properties );

                                    // Build road if it does not exist
                                    QSharedPointer< Entity > entity = environment::EntityEnvironment::globalInstance()->getByUID( entity_id );
                                    if( entity ){
//                                        qWarning() << QString("Skipping duplicate (Duplicate UID %1 found)").arg( entity_id );
                                    } else {

                                        QJsonObject linestring;
                                        linestring.insert( "type", "LineString" );
                                        linestring.insert( "coordinates", QJsonArray() );

                                        for (const auto &point : polyline) {
                                            QJsonArray p;
                                            p.push_back( point.longitude() );
                                            p.push_back( point.latitude() );
                                            linestring.value( "coordinates" ).toArray().push_back( p );
                                        }

                                        QJsonObject way_template;
                                        way_template.insert( "id", entity_id );
                                        way_template.insert( "osdm_id", osm_id );
                                        way_template.insert( "@gwsclass", "Entity");
                                        way_template.insert( "@gwsgroups", QJsonArray( { "Road" } ) );
                                        way_template.insert( "linestring", linestring );
                                        way_template.insert( "osm_type", "way" );
                                        way_template.insert( "accumulated_noise", "" );
                                        way_template.insert( "accummulated_co2", "" );
                                        way_template.insert( "accummulated_fc", "" );
                                        way_template.insert( "accummulated_fcmj", "" );

//                                        QSharedPointer<geoworldsim::Entity> entity = dynamic_cast<geoworldsim::Entity> (ObjectFactory::globalInstance()->fromJSON( way_template ));

//                                        environment::EntityEnvironment::globalInstance()->registerEntity( entity );
//                                        environment::ExecutionEnvironment::globalInstance()->registerEntity( entity );

//                                        qDebug() << "ROADS" << environment::EntityEnvironment::globalInstance()->getByClass( "Road" ).length();

//                                        qDebug() << 2;
                                    }
                                }

                                if (properties.value( TRANSPORT_MODE ).toString().toUpper() == "STATION"){
                                    properties.insert(IS_STATION, true);
                                }

                                if ( destination_coor != segment_coor ){
                                    multiroute_skill->addDestination( destination_coor , properties );
                                }
                            }
                        }
                        else if ( legs_array.isEmpty() && stop_if_no_route ){
                              environment::ExecutionEnvironment::globalInstance()->unregisterEntity( agent );
                        }
                    }

                    reply->deleteLater();
                    agent->decrementBusy();

                });

            });
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
