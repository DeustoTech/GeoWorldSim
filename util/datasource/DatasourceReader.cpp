#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GWSDatasourceReader::GWSDatasourceReader(QString scenario_id , QString entity_type, int limit ) : QObject(){
    this->scenario_id = scenario_id;
    this->entity_type = entity_type;
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

bool GWSDatasourceReader::downloadedFinished(){
    return this->downloaded_finished;
}

void GWSDatasourceReader::requestPaginated(int page){
    QString paginated_url = QString("http://history.geoworldsim.com/api/scenario/%1/entities/%2?offset=%3&limit=%4").arg( this->scenario_id ).arg( this->entity_type ).arg( page * this->page_size ).arg( this->page_size );
    qDebug() << QString("Requesting entities %1 from scenario %2, from %3 to %4").arg( this->entity_type ).arg( paginated_url ).arg( page * this->page_size ).arg( (page+1) * this->page_size );

    QNetworkReply* reply = this->api_driver.GET( paginated_url );
    reply->connect( reply , &QNetworkReply::finished , this , &GWSDatasourceReader::dataReceived );
}

void GWSDatasourceReader::dataReceived(){
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    foreach(QJsonValue j , json.value("entities").toArray() ) {
        QJsonObject data_received = j.toObject();
        if( this->downloaded_total < this->download_limit ){
            emit this->dataValueReadSignal( data_received );
            this->downloaded_data.append( data_received );
        }
        this->downloaded_total++;
    }

    unsigned int count = json.value( "count" ).toInt();
    qDebug() << QString("Downloaded datasource %1, amount %2 of total %3").arg( this->scenario_id ).arg( this->downloaded_total ).arg( count );

    if( count > (this->last_paginated+1) * this->page_size && this->downloaded_total < this->download_limit ){
        this->requestPaginated( ++this->last_paginated );
    } else {
        emit this->dataReadingFinishedSignal();
        this->downloaded_finished = true;
    }
}
