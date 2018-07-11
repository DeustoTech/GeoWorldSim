#include "ASyncAPIDriver.h"

GWSASyncAPIDriver::GWSASyncAPIDriver(QObject *parent) : GWSAPIDriver( parent ){
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QNetworkReply* GWSASyncAPIDriver::operation(QNetworkAccessManager::Operation operation, QUrl url, QMap<QString, QString> headers, QByteArray data , QByteArray custom_operation ){

    qDebug() << QString("Performing operation %1 %2").arg( operation ).arg( url.toString() );

    // Request
    QNetworkRequest request(url);

    // Add request headers
    foreach ( const QString header, headers.keys() ){
        request.setRawHeader( header.toStdString().c_str(), headers.value( header ).toStdString().c_str() );
    }

    // Add data
    QBuffer buffer;
    buffer.open( ( QBuffer::ReadWrite ) );
    buffer.write( data );
    buffer.seek( 0 );

    QNetworkReply* unfinished_reply = 0;

    try {

        if( operation == QNetworkAccessManager::GetOperation ){ unfinished_reply = this->access_manager->get( request ); }
        if( operation == QNetworkAccessManager::PostOperation ){ unfinished_reply = this->access_manager->post( request , &buffer ); }
        if( operation == QNetworkAccessManager::PutOperation ){ unfinished_reply = this->access_manager->put( request , &buffer ); }
        if( operation == QNetworkAccessManager::CustomOperation ){ unfinished_reply = this->access_manager->sendCustomRequest( request , custom_operation , &buffer ); }
        if( operation == QNetworkAccessManager::DeleteOperation ){ unfinished_reply = this->access_manager->deleteResource( request ); }

    } catch(...){
        qWarning() << QString("Crashed request %1 %2").arg( operation ).arg( url.toString() );
    }

    return unfinished_reply; // Unfinished reply, receiver will need to connect to finished signal
}
