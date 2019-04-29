#include "GTAlgBehaviour.h"

QString GTAlgBehaviour::TRIP_LEGS = "trip_legs";
QString GTAlgBehaviour::NEXT_TRIP_LEG = "next_trip_leg";
QString GTAlgBehaviour::NEXTS_IF_LEGS = "nexts_if_leg";
QString GTAlgBehaviour::NEXTS_IF_NO_LEGS = "nexts_if_no_leg";


#include "../../util/api/APIDriver.h"
#include "../../util/geometry/Coordinate.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

GTAlgBehaviour::GTAlgBehaviour() : GWSBehaviour(){

}

QPair< double , QJsonArray > GTAlgBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QJsonArray legs = agent->getProperty( this->getProperty( TRIP_LEGS ).toString() ).toArray();

    // If legs are empty, calculate them through algorithm:
    if ( legs.isEmpty() ){

        GWSCoordinate from = GWSCoordinate( 43.265175067902234,-2.934143543243408 );
        GWSCoordinate to = GWSCoordinate( 43.261393639746174,-2.94234037399292 );

        qint64 currentDateTime = GWSTimeEnvironment::globalInstance()->getCurrentDateTime();
        QDateTime timeStamp = QDateTime::fromMSecsSinceEpoch( currentDateTime );
        QDate date = timeStamp.date();
        QTime time = timeStamp.time();

        QString transport_mode = agent->getProperty( "transport_mode" ).toString();

        QString optimization_mode = "QUICKER";

        // url type to query:
        QString gtUrl = QString( "http://157.158.135.195:8081/gtalg/routers/default/plan?fromPlace=%1,%2&toPlace=%3,%4&time=%5&date=%6&mode=CAR&maxWalkDistance=750&maxBikeDistance=10000&maxElectricCarDistance=112654&weightOptimization=QUICKER&requestedResults=1&responseTimeout=3&arriveBy=false&showIntermediateStops=false&energyConsumption=16&energyCost=50&fuelConsumption=8&fuelCost=500&motorFuelConsumption=4&congestionEnabled=false&efaType=CC")
                .arg( from.getX() ).arg( from.getY() )
                .arg( to.getX() ).arg( to.getY())
                .arg( time.toString( "hh:mm") )
                .arg( date.toString( "MM-dd-yyyy") );

        qDebug() << gtUrl;

        QMetaObject::invokeMethod( GWSApp::globalInstance() , [ agent , gtUrl ]{

            QNetworkReply* reply = GWSAPIDriver::globalInstance()->GET( gtUrl );

            agent->incrementBusy(); // IMPORTANT TO WAIT UNTIL REQUEST FINISHES

            reply->connect( reply , &QNetworkReply::finished , [ agent , reply ] {

                qDebug() << "Receiving data";
                QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();


                // We are only interested in the "itineraries.legs" field of the QJsonObject

                QJsonObject plan =  json.value( "plan" ).toObject();
                QJsonArray itineraries = plan.value( "itineraries" ).toArray();

                QJsonArray legs_array;

                if ( !itineraries.isEmpty() ){

                    for ( int i = 0; i < itineraries.size() ; ++i ){

                        QJsonObject itinerariesObj = itineraries.at( i ).toObject();
                        legs_array.append( itinerariesObj.value( "legs" ));
                        }
                    }

                reply->deleteLater();

                agent->decrementBusy();
                agent->setProperty( "trip_legs" , legs_array );

            } );

        });


        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_LEGS ).toArray() );

    }

    QJsonObject next_leg = legs.at( 0 ).toObject();
    agent->setProperty( NEXT_TRIP_LEG , next_leg );
    legs.removeAt( 0 );

    if ( legs.isEmpty() ){
        return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_NO_LEGS ).toArray() );
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS_IF_LEGS ).toArray() );
}


/*******************************************
 *******************************************/
void GTAlgBehaviour::dataReceived(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    qDebug() << "Receiving data";
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    agent->decrementBusy();

}
