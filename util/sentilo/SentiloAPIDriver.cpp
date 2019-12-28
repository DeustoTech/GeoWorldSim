#include "SentiloAPIDriver.h"

#include "utils/api/SyncAPIDriver.h"
#include <QJsonDocument>

SentiloAPIDriver::SentiloAPIDriver(QString host, int port, QObject *parent ) : QObject( parent ){
    this->host = host;
    this->port = port;
}

SentiloAPIDriver::~SentiloAPIDriver(){
}

void SentiloAPIDriver::getCatalog(){
    QUrl sentilo_url = QUrl( this->host );
    sentilo_url.setPort( this->port );
    sentilo_url.setPath( this->catalog_endpoint );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "IDENTITY_KEY", this->token );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->GET( sentilo_url, headers );

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reply->readAll();
    }

    api->deleteLater();
}

void SentiloAPIDriver::createEntity(QJsonObject entity_json){

    QUrl sentilo_url = QUrl( this->host );
    sentilo_url.setPort( this->port );
    sentilo_url.setPath( this->catalog_endpoint + this->provider_endpoint );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "IDENTITY_KEY", this->token );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->POST( sentilo_url, headers, QJsonDocument( entity_json ).toJson() );

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reply->readAll();
    }

    api->deleteLater();
}

void SentiloAPIDriver::patchEntity(QJsonObject patch_json){

    QUrl sentilo_url = QUrl( this->host );
    sentilo_url.setPort( this->port );
    sentilo_url.setPath( this->catalog_endpoint + this->provider_endpoint );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "IDENTITY_KEY", this->token );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->PUT( sentilo_url, headers, QJsonDocument( patch_json ).toJson() );

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reply->readAll();
    }

    api->deleteLater();
}

void SentiloAPIDriver::deleteEntity(QJsonObject entity_json){
    QUrl sentilo_url = QUrl( this->host );
    sentilo_url.setPort( this->port );
    sentilo_url.setPath( this->catalog_endpoint + this->provider_endpoint );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "IDENTITY_KEY", this->token );

    QUrlQuery params;
    params.addQueryItem( "method", "delete" );
    sentilo_url.setQuery( params );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->PUT( sentilo_url, headers, QJsonDocument( entity_json ).toJson() );

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reply->readAll();
    }

    api->deleteLater();
}


void SentiloAPIDriver::sendObservation(QString sensor_id, QJsonObject entity_json){

    QUrl sentilo_url = QUrl( this->host );
    sentilo_url.setPort( this->port );
    sentilo_url.setPath( this->data_endpoint + this->provider_endpoint + "/" + sensor_id);

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "IDENTITY_KEY", this->token );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->PUT( sentilo_url, headers, QJsonDocument( entity_json ).toJson() );

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << "ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << reply->readAll();
    }

    api->deleteLater();
}

