#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../object/ObjectFactory.h"
#include "../../agent/Agent.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"

GWSDatasourceReader::GWSDatasourceReader(QString datasource_url) : QObject(){
    this->datasource_url = datasource_url;

    this->requestPaginated( this->last_paginated );
}

void GWSDatasourceReader::requestPaginated(int page){
    QString paginated_url = this->datasource_url + QString("?offset=%1&limit=%2").arg( page * this->limit ).arg( this->limit );

    QNetworkReply* reply = this->api_driver.GET( paginated_url );
    reply->connect( reply , &QNetworkReply::finished , this , &GWSDatasourceReader::dataReceived );
}

void GWSDatasourceReader::dataReceived(){
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    foreach(QJsonValue j , json.value("data").toArray() ) {
        GWSAgent* agent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( j.toObject() ) );
        if( agent ){
            // TODO
            GWSAgentEnvironment::globalInstance()->registerAgent( agent );
            GWSGridEnvironment::globalInstance()->registerAgent( agent );
            GWSNetworkEnvironment::globalInstance()->registerAgent( agent );
            GWSPhysicalEnvironment::globalInstance()->registerAgent( agent );
        }
    }

    unsigned int count = json.value( "count" ).toInt();
    if( count > this->last_paginated * this->limit ){
        this->requestPaginated( ++this->last_paginated );
    }
}
