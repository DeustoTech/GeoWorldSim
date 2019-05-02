#include "GTAlgBehaviour.h"

QString GTAlgBehaviour::DESTINATION_X = "destination_x";
QString GTAlgBehaviour::DESTINATION_Y = "destination_y";
QString GTAlgBehaviour::TRIP_LEGS = "store_trip_legs_as";
QString GTAlgBehaviour::CURRENT_LEG_MODE = "store_current_leg_mode_as";
QString GTAlgBehaviour::CURRENT_LEG_TO_X = "store_current_leg_destination_x_as";
QString GTAlgBehaviour::CURRENT_LEG_TO_Y = "store_current_leg_destination_y_as";
QString GTAlgBehaviour::NEXTS_IF_LEGS = "nexts_if_leg";
QString GTAlgBehaviour::NEXTS_IF_NO_LEGS = "nexts_if_no_leg";
QString GTAlgBehaviour::NEXTS_IF_WAITING = "nexts_if_waiting";


#include "../../util/api/APIDriver.h"
#include "../../util/geometry/Coordinate.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"


GTAlgBehaviour::GTAlgBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > GTAlgBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QJsonArray legs = agent->getProperty( this->getProperty( TRIP_LEGS ).toString() ).toArray();

    // If legs are empty, calculate them through algorithm:
    if ( legs.isEmpty() ){

        double from_y = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent ).getCentroid().getX();
        double from_x = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent ).getCentroid().getY();

        double to_y = this->getProperty( DESTINATION_X ).toDouble() ;
        double to_x = this->getProperty( DESTINATION_Y ).toDouble() ;


        qint64 currentDateTime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
        QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch( currentDateTime );
        QDate date = timeStamp.date();
        QTime time = timeStamp.time();

        QString transport_mode = agent->getProperty( "transport_mode" ).toString();

        QString optimization_mode = "QUICKER";

        // url type to query:
        QString gtUrl = QString( "http://157.158.135.195:8081/gtalg/routers/default/plan?fromPlace=%1,%2&toPlace=%3,%4&time=%5&date=%6&mode=CAR&maxWalkDistance=750&maxBikeDistance=10000&maxElectricCarDistance=112654&weightOptimization=QUICKER&requestedResults=1&responseTimeout=3&arriveBy=false&showIntermediateStops=false&energyConsumption=16&energyCost=50&fuelConsumption=8&fuelCost=500&motorFuelConsumption=4&congestionEnabled=false&efaType=CC")
                .arg( from_x ).arg( from_y )
                .arg( to_x ).arg( to_y)
                .arg( time.toString( "hh:mm") )
                .arg( date.toString( "MM-dd-yyyy") );

       //qDebug() << gtUrl;

        QMetaObject::invokeMethod( GWSApp::globalInstance() , [ agent , gtUrl , this]{

            QNetworkReply* reply = GWSAPIDriver::globalInstance()->GET( gtUrl );

            agent->incrementBusy(); // IMPORTANT TO WAIT UNTIL REQUEST FINISHES

            reply->connect( reply , &QNetworkReply::finished , [ agent , reply , this ] {


                QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();


                // We are only interested in the "itineraries.legs" field of the QJsonObject

                if ( !json.isEmpty() ){

                    qDebug() << "Receiving data";

                    QJsonObject plan =  json.value( "plan" ).toObject();
                    QJsonArray itineraries = plan.value( "itineraries" ).toArray();
                    QJsonObject optimal_itinerary = itineraries.at( 0 ).toObject();
                    QJsonValue legs_array =  optimal_itinerary.value( "legs" ).toArray() ;

                    reply->deleteLater();

                    agent->decrementBusy();
                    agent->setProperty( this->getProperty( TRIP_LEGS ).toString() , legs_array );
                }

            } );


        });

        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_WAITING ).toArray() );

    }


    QJsonObject current_leg = legs.at( 0 ).toObject();
    QString leg_mode = current_leg.value( "mode").toString();
    QJsonObject leg_to = current_leg.value( "to").toObject();

    double leg_x = leg_to.value( "lon" ).toDouble();
    double leg_y = leg_to.value( "lat" ).toDouble();

    int leg_departure = leg_to.value( "departure" ).toInt();


    // Store current leg mode, from, to and departure time in agent:
    agent->setProperty( this->getProperty( CURRENT_LEG_MODE ).toString() , leg_mode );
    agent->setProperty( this->getProperty( CURRENT_LEG_TO_X ).toString() , leg_x );
    agent->setProperty( this->getProperty( CURRENT_LEG_TO_Y ).toString() , leg_y );

    GWSTimeEnvironment::globalInstance()->setAgentInternalTime( agent , leg_departure );

    legs.removeAt( 0 );
    agent->setProperty( this->getProperty( TRIP_LEGS ).toString() , legs );

    // TO DO: GESTIONAR CASO DE UN SOLO LEG.

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_LEGS ).toArray() );


}


/*******************************************
 *  METHODS
 *******************************************/
void GTAlgBehaviour::dataReceived(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    qDebug() << "Receiving data";
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    agent->decrementBusy();

}
