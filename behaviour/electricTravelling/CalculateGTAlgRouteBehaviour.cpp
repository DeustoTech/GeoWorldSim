#include "CalculateGTAlgRouteBehaviour.h"

QString CalculateGTAlgRouteBehaviour::GTALG_HOST = "gtalg_host";
QString CalculateGTAlgRouteBehaviour::DESTINATION_X = "destination_x";
QString CalculateGTAlgRouteBehaviour::DESTINATION_Y = "destination_y";
QString CalculateGTAlgRouteBehaviour::DESTINATION_JSON  = "destination_json";
QString CalculateGTAlgRouteBehaviour::TRANSPORT_MODE = "transport_mode";
QString CalculateGTAlgRouteBehaviour::OPTIMIZATION = "route_optimization";
QString CalculateGTAlgRouteBehaviour::STOP_ENTITY_IF_NO_ROUTE = "stop_if_no_route";

QString CalculateGTAlgRouteBehaviour::NEXTS = "nexts";


#include "../../util/api/APIDriver.h"
#include "../../util/geometry/Coordinate.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../skill/move/StoreMultiRouteSkill.h"


CalculateGTAlgRouteBehaviour::CalculateGTAlgRouteBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > CalculateGTAlgRouteBehaviour::behave(){

    QSharedPointer<GWSEntity> agent = this->getEntity();
    QJsonArray next_destinations = agent->getProperty( this->getProperty( GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ).toString( GWSStoreMultiRouteSkill::PENDING_ROUTE_DESTINATIONS ) ).toArray();



    // If legs are empty, calculate them through algorithm:
    if ( next_destinations.isEmpty() ){

        GWSGeometry agent_geom = GWSGeometry( agent->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).toObject() );
        GWSCoordinate agent_coor = agent_geom.getCentroid();

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

            GWSGeometry geom_json = GWSGeometry( json_value.toObject() );
            GWSCoordinate coor_json = geom_json.getCentroid();

            dest_x = coor_json.getX();
            dest_y = coor_json.getY();

        }

        qint64 currentDateTime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
        QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch( currentDateTime );
        QDate date = timeStamp.date();
        QTime time = timeStamp.time();

        // url type to query:
        QString gtalg_host = this->getProperty( "gtalg_host" ).toString( "http://157.158.135.195:8081/gtalg/routers/default" );

        QString gtUrl = QString(  gtalg_host + "/plan?fromPlace=%1,%2&toPlace=%3,%4&time=%5&date=%6&mode=%7&maxWalkDistance=750&maxBikeDistance=10000&maxElectricCarDistance=112654&weightOptimization=%8&requestedResults=1&responseTimeout=3&arriveBy=false&showIntermediateStops=false&energyConsumption=16&energyCost=50&fuelConsumption=8&fuelCost=500&motorFuelConsumption=4&congestionEnabled=false&efaType=CC")
                .arg( from_y ).arg( from_x )
                .arg( dest_y ).arg( dest_x )
                .arg( time.toString( "hh:mm" ) )
                .arg( date.toString( "MM-dd-yyyy" ) )
                .arg( this->getProperty( TRANSPORT_MODE ).toString() )
                .arg( this->getProperty( OPTIMIZATION ).toString() );

        agent->incrementBusy(); // IMPORTANT TO WAIT UNTIL REQUEST FINISHES

        QTimer::singleShot( qrand() % 1000 , GWSApp::globalInstance() , [ agent , gtUrl , this ]{

            GWSAPIDriver::globalInstance()->GET( gtUrl , [ agent , this ]( QNetworkReply* reply ){

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

                            // Prepare the next_destinations as required by GWSStoreMultiRouteSkill
                            QSharedPointer<GWSStoreMultiRouteSkill> multiroute_skill = agent->getSkill( GWSStoreMultiRouteSkill::staticMetaObject.className() , true ).dynamicCast<GWSStoreMultiRouteSkill>();
                            if( !multiroute_skill ){
                                multiroute_skill = QSharedPointer<GWSStoreMultiRouteSkill>( new GWSStoreMultiRouteSkill() );
                                agent->addSkill( multiroute_skill );
                            }

                            // Extract array of ordered legs
                            foreach( QJsonValue v , legs_array ){

                                QJsonObject leg = v.toObject();
                                QMap<QString,QString> colors = {{"WALK","Grey"},{"SUBWAY","Yellow"},{"TRAM","Pink"},{"BUS","Blue"},{"MOTORCYCLE","Orange"},{"CAR","Red"},{"ELECTRIC","Lime"}};

                                GWSCoordinate destination_coor( leg.value( "to" ).toObject().value( "lon" ).toDouble() , leg.value( "to" ).toObject().value( "lat" ).toDouble() );

                                // Additional properties
                                QJsonObject properties;
                                properties.insert( TRANSPORT_MODE , leg.value("mode").toString() );
                                //properties.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , leg.value( "startTime" ).toDouble() );
                                properties.insert( "color" , colors.value( leg.value( "mode" ).toString() , "Black" ) );

                                multiroute_skill->addDestination( destination_coor , properties );
                            }
                        }
                        else if ( legs_array.isEmpty() && stop_if_no_route ){

                              GWSExecutionEnvironment::globalInstance()->unregisterEntity( agent );


                        }
                    }

                    reply->deleteLater();
                    agent->decrementBusy();

                });


            });


        });

    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
