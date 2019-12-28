#include "IOTAgent.h"

#include <QJsonDocument>
#include <QDebug>

IOTAgent::IOTAgent(QString orion_url, int orion_port, int iotagent_server_listen_port , QObject *parent) : QObject(parent){

    // Create Orion Api to make calls to Orion
    this->orion_api = new OrionAPIDriver( orion_url , orion_port );
    this->iotagent_server = 0;
    this->iotagent_server_listen_port = iotagent_server_listen_port;

    // Listen to notifications from Orion
    if( this->iotagent_server_listen_port > 0 ){
        this->iotagent_server = new QHttpServer( this );
        this->iotagent_server->listen(QHostAddress::Any , this->iotagent_server_listen_port , [&](QHttpRequest* req, QHttpResponse* res){

            Q_UNUSED(res)
            qDebug() << "REQUEST RECEIVED FROM ORION" << req;
            qDebug() << "EMIT SIGNAL WITH THE DATA";

            req->deleteLater();
        });

        if ( this->iotagent_server->isListening() ){
            qDebug() << "[IOTAgent::IOTAgent] IOTAgent server initiated in port " << this->iotagent_server_listen_port << endl;
        } else {
            qWarning() << "[IOTAgent::IOTAgent] IOTAgent server could not be initiated in port " << this->iotagent_server_listen_port << endl;
        }
    }

}

IOTAgent::~IOTAgent(){
    if( this->iotagent_server ){ this->iotagent_server->deleteLater(); }
    this->orion_api->deleteLater();
}

/**********************************************************************
 METHODS
**********************************************************************/

QList<QJsonObject> IOTAgent::getEntities(){
    return this->orion_api->getEntities();
}

void IOTAgent::getEntityById( QString entity_id, QString entity_type){
    this->orion_api->getEntityById( entity_id, entity_type );
}

void IOTAgent::getEntitiesByType( QString entity_type){
    this->orion_api->getEntitiesByType( entity_type );
}

void IOTAgent::createEntity(QString domain_path, QJsonObject entity_json) {
    this->orion_api->createEntity( domain_path , entity_json );
}

void IOTAgent::patchEntity(QString domain_path, QString entity_id, QString entity_tpe, QJsonObject patch_json){
    this->orion_api->patchEntity( domain_path , entity_id , entity_tpe, patch_json );
}

void IOTAgent::deleteEntity(QString domain_path, QString entity_id, QString entity_type ){
    this->orion_api->deleteEntity( domain_path , entity_id, entity_type );
}


void IOTAgent::createSubscription(QString description , QString entity_type , QString entity_id , QStringList condition_attributes , QStringList notify_attributes ){

    QJsonObject subscription_subject;
    QJsonArray entities;
    QJsonObject entity;
    entity.insert( "id" , entity_id );
    entity.insert( "type" , entity_type );
    entities.append( entity );
    subscription_subject.insert( "entities" , entities );

    QJsonObject condition;
    QJsonArray attrs_condition;
    foreach(QString a , condition_attributes){
        attrs_condition.append( a );
    }
    condition.insert( "attrs" , attrs_condition );
    subscription_subject.insert( "condition" , condition );

    QJsonObject subscription_notification;
    QJsonObject http;
    http.insert( "url" , QString( "http://%1:%2" ).arg( "localhost" ).arg( this->iotagent_server_listen_port ) );
    subscription_notification.insert( "http" , http );

    QJsonArray notifications;
    foreach (QString a, notify_attributes) {
        notifications.append( a );
    }
    subscription_notification.insert( "attrs" , notifications );

    QJsonObject subscription;
    subscription.insert( "description" , description );
    subscription.insert( "subject" , subscription_subject );
    subscription.insert( "notification" , subscription_notification );
    subscription.insert( "expires" , "2040-01-01T14:00:00.00Z" );
    subscription.insert( "throttling" , 1 );

    this->orion_api->createSubscription( subscription );
}

void IOTAgent::deleteSubscription(QString subscription_id){
    this->orion_api->deleteSubscription( subscription_id );
}

void IOTAgent::sendObservation(QString domain_path , QString entity_id , QString entity_type, QJsonObject patch_json){
    this->patchEntity(domain_path, entity_id, entity_type, patch_json);
}

