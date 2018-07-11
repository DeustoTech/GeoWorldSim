#include "OrionAPIDriver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

OrionAPIDriver::OrionAPIDriver( QString host, int port, QObject *parent ) : QObject( parent ){
    this->host = host;
    this->port = port;
    this->token = "";

    qDebug() << "[OrionAPIDriver] Connected to Orion" << this->checkVersion();
}

OrionAPIDriver::~OrionAPIDriver(){
}


/**********************************************************************
 GETTERS
**********************************************************************/

QString OrionAPIDriver::checkVersion(){

    QString version;

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->version_endpoint );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->GET( orion_url, headers );
    api->deleteLater();

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << QJsonDocument::fromJson( reply->readAll() );
    } else {        
        QJsonObject res = QJsonDocument::fromJson( reply->readAll() ).object();
        QJsonObject orion = res.value( "orion" ).toObject();
        version = orion.value( "version" ).toString();
    }
    reply->deleteLater();
    return version;
}


QList<QJsonObject> OrionAPIDriver::getEntities(){

    QList<QJsonObject> entities;

    // Set request path for entities
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->GET( orion_url, headers );
    api->deleteLater();

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << QJsonDocument::fromJson( reply->readAll() );
    } else {
        QJsonArray array = QJsonDocument::fromJson( reply->readAll() ).array();
        foreach( QJsonValue value, array ){
            entities.append( value.toObject() );
        }
    }

    reply->deleteLater();
    return entities;
}

QList<QJsonObject> OrionAPIDriver::getEntityById( QString entity_id, QString entity_type ) {

    QList<QJsonObject> entities;

    // Set request path for entities
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );

    QUrlQuery params;
    params.addQueryItem( "id", entity_id );
    params.addQueryItem( "type", entity_type );
    orion_url.setQuery( params );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->GET( orion_url, headers );
    api->deleteLater();

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << QJsonDocument::fromJson( reply->readAll() );
    } else {
        QJsonArray array = QJsonDocument::fromJson( reply->readAll() ).array();
        foreach( QJsonValue value, array ){
            entities.append( value.toObject() );
        }
    }
    reply->deleteLater();
    return entities;
}

QList<QJsonObject> OrionAPIDriver::getEntitiesByType( QString entities_type ) {

    QList<QJsonObject> entities;

    // Set request path for entities
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );

    QUrlQuery params;
    params.addQueryItem( "type", entities_type );
    orion_url.setQuery( params );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->GET( orion_url, headers );
    api->deleteLater();

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << Q_FUNC_INFO << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << QJsonDocument::fromJson( reply->readAll() );
    }
    else{
        QJsonArray array = QJsonDocument::fromJson( reply->readAll() ).array();
        foreach( QJsonValue value, array ){
            entities.append( value.toObject() );
        }
    }
    reply->deleteLater();
    return entities;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void OrionAPIDriver::createEntity(QString domain_path, QJsonObject entity_json){

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );
    headers.insert( "Fiware-ServicePath", domain_path );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->POST( orion_url, headers, QJsonDocument( entity_json ).toJson() );

    api->deleteLater();
}

void OrionAPIDriver::patchEntity( QString domain_path , QString entity_id , QString entity_type, QJsonObject patch_json ){
    Q_UNUSED(entity_type);

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint + "/" + entity_id + "/attrs" );

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );
    headers.insert( "Fiware-ServicePath", domain_path );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->PATCH( orion_url, headers, QJsonDocument( patch_json ).toJson() );
    reply->deleteLater();
    api->deleteLater();
}

void OrionAPIDriver::deleteEntity( QString domain_path , QString entity_id, QString entity_type ){

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->entities_endpoint + "/" + entity_id );;

    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );
    headers.insert( "Fiware-ServicePath", domain_path );

    QUrlQuery params;
    params.addQueryItem( "type", entity_type );
    orion_url.setQuery( params );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    api->DELETE( orion_url, headers );
    api->deleteLater();
}


void OrionAPIDriver::createSubscription( QJsonObject subscription_json ){

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->subscriptions_endpoint );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "Content-Type", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    api->POST( orion_url , headers, QJsonDocument( subscription_json ).toJson() );
    api->deleteLater();
}


void OrionAPIDriver::deleteSubscription(QString subscription_id){

    // Set request path for services
    QUrl orion_url = QUrl( host );
    orion_url.setPort( port );
    orion_url.setPath( this->subscriptions_endpoint + "/" + subscription_id );

    // Add request headers
    QMap<QString, QString> headers = QMap<QString, QString>();
    headers.insert( "accept", "application/json" );
    headers.insert( "X-Auth-Token", this->token );
    headers.insert( "Fiware-Service", this->fiware_service );

    SyncAPIDriver* api = new SyncAPIDriver( this->parent() ? this->parent() : this );
    QNetworkReply* reply = api->DELETE( orion_url, headers );
    api->deleteLater();

    if ( reply->error() != QNetworkReply::NoError ){
        qDebug() << "[OrionAPIDriver::setSubscription] ERROR:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << QJsonDocument::fromJson( reply->readAll() );
    }
    reply->deleteLater();
}
