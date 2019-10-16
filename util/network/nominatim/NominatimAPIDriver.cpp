#include "NominatimAPIDriver.h"

NominatimAPIDriver::NominatimAPIDriver(QObject *parent) : QObject(parent){
}

NominatimAPIDriver::~NominatimAPIDriver(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void NominatimAPIDriver::getLocation(QString place_name){

    // http://wiki.openstreetmap.org/wiki/Nominatim

    QString query = QString("http://nominatim.openstreetmap.org/search?q=%1&format=json&polygon=1&addressdetails=1")
            .arg( place_name );

    SyncAPIDriver* api = new SyncAPIDriver( this );
    QNetworkReply* reply = api->GET( QUrl( query ) );

    // If the request returned an error
    if( reply->error() != QNetworkReply::NoError ){
        qWarning() << Q_FUNC_INFO << "Network error" << reply->errorString() << endl;
        reply->deleteLater();
        return;
    }

    // Else parse reply
    // TODO

    if( reply ){ reply->deleteLater(); }
    api->deleteLater();

}

void NominatimAPIDriver::getPlaceInfo(geos::geom::Coordinate coor , int zoom){

    // http://wiki.openstreetmap.org/wiki/Nominatim

    QString query = QString("http://nominatim.openstreetmap.org/reverse?format=json&lat=%1&lon=%2&zoom=%3&addressdetails=1")
            .arg( coor.y )
            .arg( coor.x )
            .arg( zoom );

    SyncAPIDriver* api = new SyncAPIDriver( this );
    QNetworkReply* reply = api->GET( QUrl( query ) );

    // If the request returned an error
    if( reply->error() != QNetworkReply::NoError ){
        qWarning() << Q_FUNC_INFO << "Network error" << reply->errorString() << endl;
        reply->deleteLater();
        return;
    }

    // Else parse reply
    // TODO

    if( reply ){ reply->deleteLater(); }
    api->deleteLater();

    return;
}
