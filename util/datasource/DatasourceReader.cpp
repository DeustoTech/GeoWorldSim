#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../util/network/HttpDriver.h"

geoworldsim::datasource::DatasourceReader::DatasourceReader(QString user_id , QString scenario_id , QString entities_type, QString entities_filter, int limit ) : QObject(){
    this->scenario_id = scenario_id;
    this->user_id = user_id;
    this->entity_type = entities_type;
    this->entity_filter = entities_filter;
    this->download_limit = limit;
}

void geoworldsim::datasource::DatasourceReader::startReading(){
    this->last_paginated = 0;
    this->downloaded_total = 0;
    this->requestPaginated( this->last_paginated );
}

QJsonArray geoworldsim::datasource::DatasourceReader::getDownloadedData(){
    //return this->downloaded_data;
    return QJsonArray();
}

bool geoworldsim::datasource::DatasourceReader::downloadedFinished(){
    return this->downloaded_finished;
}

void geoworldsim::datasource::DatasourceReader::requestPaginated(int page){
    QString paginated_url = QString("https://history.geoworldsim.com/api/scenario/%1/entities/%2?offset=%3&limit=%4&user_id=%5&attributes=*&%6").arg( this->scenario_id ).arg( this->entity_type ).arg( page * this->page_size ).arg( this->page_size ).arg( this->user_id ).arg( this->entity_filter );

    QString message = QString("Requesting %1 from datasource %2, %3 / %4").arg( this->entity_type ).arg( this->scenario_id ).arg( page * this->page_size ).arg( (page+1) * this->page_size );
    qDebug() << message;

    emit environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                QJsonObject({ { "message" , message } }) , App::globalInstance()->getAppId() + "-LOG" );

    network::HttpDriver::globalInstance()->GET( paginated_url , [this]( QNetworkReply* reply ){
        reply->connect( reply , &QNetworkReply::finished , this , &geoworldsim::datasource::DatasourceReader::dataReceived );
    });

}

void geoworldsim::datasource::DatasourceReader::dataReceived(){
    QNetworkReply* reply = dynamic_cast<QNetworkReply*>( sender() );
    QJsonObject json = QJsonDocument::fromJson( reply->readAll() ).object();
    reply->deleteLater();

    foreach(QJsonValue j , json.value("entities").toArray() ) {
        QJsonObject data_received = j.toObject();
        if( this->downloaded_total < this->download_limit ){
            emit this->dataValueReadSignal( data_received );
           // this->downloaded_data.append( data_received );
        }
        this->downloaded_total++;
    }

    unsigned int count = json.value( "count" ).toInt();
    QString message = QString("Downloaded %1 from datasource %2, %3 / %4").arg( this->entity_type ).arg( this->scenario_id ).arg( this->downloaded_total ).arg( count );
    qDebug() << message;
    emit environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                QJsonObject({ { "message" , message } }) , App::globalInstance()->getAppId() + "-LOG" );

    if( count > (this->last_paginated+1) * this->page_size && this->downloaded_total < this->download_limit ){
        this->requestPaginated( ++this->last_paginated );
    } else {
        emit this->dataReadingFinishedSignal();
        this->downloaded_finished = true;
    }
}
