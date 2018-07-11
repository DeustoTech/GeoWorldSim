#include "APIDriver.h"
#include <QDebug>
#include <QNetworkProxyFactory>
#include <QNetworkProxy>

GWSAPIDriver::GWSAPIDriver(QObject *parent) : GWSObject( parent ){

    QNetworkProxyQuery npq( QUrl( "http://www.google.com" ) );
    QList<QNetworkProxy> proxies_list = QNetworkProxyFactory::systemProxyForQuery( npq );

    if( proxies_list.isEmpty() ){
        qWarning() << Q_FUNC_INFO << "No proxy found.";
    }

    this->access_manager = new QNetworkAccessManager();
    this->access_manager->setProxy( proxies_list.at(0) );
}

GWSAPIDriver::~GWSAPIDriver(){
    this->access_manager->deleteLater();
}

/**********************************************************************
 GET
**********************************************************************/

/**
 * WARNING! Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncGET
 * @param url
 * @return
 */
QNetworkReply* GWSAPIDriver::GET( QUrl url, QMap<QString, QString> headers ){
    return this->operation( QNetworkAccessManager::GetOperation , url , headers );
}

/**********************************************************************
 POST
**********************************************************************/

/**
   WARNING, Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncPOST
 * @param url
 * @return
 */
QNetworkReply* GWSAPIDriver::POST(QUrl url, QMap<QString, QString> headers, QByteArray data ){
    return this->operation( QNetworkAccessManager::PostOperation , url , headers , data );
}

/**********************************************************************
 PUT
**********************************************************************/

/**
   WARNING, Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncPATCH
 * @param url
 * @return
 */
QNetworkReply* GWSAPIDriver::PUT(QUrl url, QMap<QString, QString> headers, QByteArray data ){
    return this->operation( QNetworkAccessManager::PutOperation , url , headers , data );
}

/**********************************************************************
 PATCH
**********************************************************************/

/**
   WARNING, Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncPATCH
 * @param url
 * @return
 */
QNetworkReply* GWSAPIDriver::PATCH(QUrl url, QMap<QString, QString> headers, QByteArray data ){
    return this->operation( QNetworkAccessManager::CustomOperation , url , headers , data , "PATCH" );
}

/**********************************************************************
 DELETE
**********************************************************************/

/**
 * WARNING! Caller gets ownership of QNetworkReply and must delete it
 * @brief APIDriver::syncGET
 * @param url
 * @return
 */
QNetworkReply* GWSAPIDriver::DELETE( QUrl url, QMap<QString, QString> headers ){
    return this->operation( QNetworkAccessManager::DeleteOperation , url , headers );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

QNetworkReply* GWSAPIDriver::operation(QNetworkAccessManager::Operation operation, QUrl url, QMap<QString, QString> headers, QByteArray data, QByteArray custom_operation){

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

