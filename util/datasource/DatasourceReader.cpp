#include "DatasourceReader.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../../util/api/APIDriver.h"

GWSDatasourceReader::GWSDatasourceReader(QString scenario_id , QString entities_type, QString entities_filter, int limit ) : QObject(){
    this->scenario_id = scenario_id;
    this->entities_type = entities_type;
    this->entities_filter = entities_filter;
    this->download_limit = limit;
}

void GWSDatasourceReader::startReading(){
    this->last_paginated = 0;
    this->downloaded_total = 0;
    this->requestPaginated( this->last_paginated );
}

QJsonArray GWSDatasourceReader::getDownloadedData(){
    //return this->downloaded_data;
    return QJsonArray();
}

bool GWSDatasourceReader::downloadedFinished(){
    return this->downloaded_finished;
}

void GWSDatasourceReader::requestPaginated(int page){
    QString paginated_url = QString("https://history.geoworldsim.com/api/scenario/%1/entities/%2?offset=%3&limit=%4&attributes=*&%5").arg( this->scenario_id ).arg( this->entities_type ).arg( page * this->page_size ).arg( this->page_size ).arg( this->entities_filter );
    qDebug() << QString("Requesting %1 from datasource %2, %3 / %4").arg( this->entities_type ).arg( this->scenario_id ).arg( page * this->page_size ).arg( (page+1) * this->page_size );

    // EXECUTE WHEN POOL HAS AVAILABLE THREADS
   // QtConcurrent::run( [ this , paginated_url ](){

        // BUT, EXECUTE IN THE MAIN THREAD
       // QTimer::singleShot( 0 , GWSAPIDriver::globalInstance() , [ this , paginated_url ](){
            QNetworkReply* reply = GWSAPIDriver::globalInstance()->GET( paginated_url );
            reply->connect( reply , &QNetworkReply::finished , this , &GWSDatasourceReader::dataReceived );
      //  });

  //  });
}

void GWSDatasourceReader::dataReceived(){
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
    qDebug() << QString("Downloaded %1 from datasource %2, %3 / %4").arg( this->entities_type ).arg( this->scenario_id ).arg( this->downloaded_total ).arg( count );

    if( count > (this->last_paginated+1) * this->page_size && this->downloaded_total < this->download_limit ){
        this->requestPaginated( ++this->last_paginated );
    } else {
        emit this->dataReadingFinishedSignal();
        this->downloaded_finished = true;
    }
}
