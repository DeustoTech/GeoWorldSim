#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../object/ObjectFactory.h"
#include "../../agent/Agent.h"
#include "../../environment/Environment.h"

GWSDatasourceReader::GWSDatasourceReader(QString datasource_url) : QObject(){
    this->datasource_url = datasource_url;
}

void GWSDatasourceReader::startReading(){
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
        QJsonObject data_received = j.toObject();
        emit this->dataValueReadSignal( data_received );
    }

    unsigned int count = json.value( "count" ).toInt();
    if( count > this->last_paginated * this->limit ){
        this->requestPaginated( ++this->last_paginated );
    } else {
        emit this->dataReadingFinishedSignal();
    }
}
