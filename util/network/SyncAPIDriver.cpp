#include "SyncAPIDriver.h"

#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QDebug>

SyncAPIDriver::SyncAPIDriver(QObject *parent) : GWSAPIDriver( parent ){

}

/**********************************************************************
 PRIVATE
**********************************************************************/

/**
 * WARNING! Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncOperation
 * @param operation
 * @param url
 * @param headers
 * @param data
 * @param custom_operation
 * @return
 */
QNetworkReply* SyncAPIDriver::operation(QNetworkAccessManager::Operation operation, QUrl url, QMap<QString, QString> headers, QByteArray data , QByteArray custom_operation ){

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

    QNetworkReply* reply = 0;

    try {

        if( operation == QNetworkAccessManager::GetOperation ){ reply = this->access_manager->get( request ); }
        if( operation == QNetworkAccessManager::PostOperation ){ reply = this->access_manager->post( request , &buffer ); }
        if( operation == QNetworkAccessManager::PutOperation ){ reply = this->access_manager->put( request , &buffer ); }
        if( operation == QNetworkAccessManager::CustomOperation ){ reply = this->access_manager->sendCustomRequest( request , custom_operation , &buffer ); }
        if( operation == QNetworkAccessManager::DeleteOperation ){ reply = this->access_manager->deleteResource( request ); }

        QEventLoop* loop = new QEventLoop( 0 );
        QTimer::singleShot( this->timeout * 1000 , loop , &QEventLoop::quit ); // Watchog
        connect( reply, &QNetworkReply::finished , loop , &QEventLoop::quit );
        loop->exec();
        loop->deleteLater();

    } catch (...) {
        qWarning() << QString("Crashed request %1 %2").arg( operation ).arg( url.toString() );
    }

    return reply;
}
