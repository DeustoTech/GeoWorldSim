#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GWSDatasourceReader::GWSDatasourceReader(QString datasource_url , int limit ) : QObject(){
    this->datasource_url = datasource_url;
    this->download_limit = limit;
}

void GWSDatasourceReader::startReading(){
    this->last_paginated = 0;
    this->downloaded_total = 0;
    this->requestPaginated( this->last_paginated );
}

QJsonArray GWSDatasourceReader::getDownloadedData(){
    return this->downloaded_data;
}

void GWSDatasourceReader::requestPaginated(int page){
    QString paginated_url = this->datasource_url + QString("?offset=%1&limit=%2").arg( page * this->page_size ).arg( this->page_size );

    QNetworkReply* reply = this->api_driver.GET( paginated_url );
    reply->connect( reply , &QNetworkReply::finished , this , &GWSDatasourceReader::dataReceived );
}

void GWSDatasourceReader::dataReceived(){
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    foreach(QJsonValue j , json.value("data").toArray() ) {
        QJsonObject data_received = j.toObject();
        if( this->downloaded_total < this->download_limit ){
            emit this->dataValueReadSignal( data_received );
            this->downloaded_data.append( data_received );
        }
        this->downloaded_total++;
    }

    unsigned int count = json.value( "count" ).toInt();
    if( count > (this->last_paginated+1) * this->page_size && this->downloaded_total < this->download_limit ){
        this->requestPaginated( ++this->last_paginated );
    } else {
        emit this->dataReadingFinishedSignal();
    }
}
