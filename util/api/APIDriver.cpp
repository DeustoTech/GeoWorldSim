#include "APIDriver.h"
#include <QDebug>
#include <QNetworkProxyFactory>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

GWSAPIDriver* GWSAPIDriver::globalInstance(){
    static GWSAPIDriver instance;
    return &instance;
}

GWSAPIDriver::GWSAPIDriver() : QObject(){

    QNetworkProxyQuery npq( QUrl( "https://www.google.com" ) );
    QList<QNetworkProxy> proxies_list = QNetworkProxyFactory::systemProxyForQuery( npq );

    if( proxies_list.isEmpty() ){
        qWarning() << Q_FUNC_INFO << "No proxy found.";
    }

    this->access_manager = new QNetworkAccessManager();
    this->access_manager->setProxy( proxies_list.at(0) );
}

GWSAPIDriver::GWSAPIDriver(const GWSAPIDriver &other) : QObject(){
    Q_UNUSED( other );
}

GWSAPIDriver::~GWSAPIDriver(){
    this->access_manager->deleteLater();
}

/**********************************************************************
 GETTERS
**********************************************************************/

int GWSAPIDriver::getRequestsAmount() const{
    return this->current_requests_amount;
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
void GWSAPIDriver::GET( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers ){
    this->operation( QNetworkAccessManager::GetOperation , url , callback , headers );
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
void GWSAPIDriver::POST(QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QByteArray data ){
    this->operation( QNetworkAccessManager::PostOperation , url , callback , headers , data );
}

void GWSAPIDriver::POST(QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QJsonObject data){
    this->POST( url , callback , headers , QJsonDocument( data ).toJson() );
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
void GWSAPIDriver::PUT(QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QByteArray data ){
    this->operation( QNetworkAccessManager::PutOperation , url , callback , headers , data );
}

void GWSAPIDriver::PUT(QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QJsonObject data){
    this->POST( url , callback , headers , QJsonDocument( data ).toJson() );
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
void GWSAPIDriver::PATCH(QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QByteArray data ){
    this->operation( QNetworkAccessManager::CustomOperation , url , callback , headers , data , "PATCH" );
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
void GWSAPIDriver::DELETE( QUrl url , std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers ){
    this->operation( QNetworkAccessManager::DeleteOperation , url , callback , headers );
}

/**********************************************************************
 PROTECTED
**********************************************************************/

void GWSAPIDriver::operation(QNetworkAccessManager::Operation operation, QUrl url, std::function<void(QNetworkReply*)> callback , QMap<QString, QString> headers, QByteArray data , QByteArray custom_operation ){

    GWSAPIDriverElement pending_request;

    // Request
    pending_request.request = QNetworkRequest(url);
    QSslConfiguration sslConfiguration( QSslConfiguration::defaultConfiguration() );
    sslConfiguration.setProtocol( QSsl::TlsV1_2 );
    pending_request.request.setSslConfiguration( sslConfiguration );

    // Operation
    pending_request.operation = operation;

    // Add request headers
    foreach ( const QString header, headers.keys() ){
        pending_request.request.setRawHeader( header.toStdString().c_str(), headers.value( header ).toStdString().c_str() );
    }

    // Add data
    pending_request.data = data;

    // Add callback
    pending_request.callback = callback;

    this->mutex.lockForRead();
    if( this->current_requests_amount > QThreadPool::globalInstance()->maxThreadCount() ){
        this->mutex.unlock();

        this->mutex.lockForWrite();
        this->pending_requests.append( pending_request );
        this->mutex.unlock();

    } else {
        this->mutex.unlock();

        QtConcurrent::run([this , &pending_request] {
            this->executePendingOperation( pending_request );
        });
    }

}

void GWSAPIDriver::executePendingOperation( GWSAPIDriverElement& pending ){

    try {

        QNetworkReply* ref_reply = Q_NULLPTR;
        QBuffer data;
        data.open( ( QBuffer::ReadWrite ) );
        data.write( pending.data );
        data.seek( 0 );

        if( pending.operation == QNetworkAccessManager::GetOperation ){ ref_reply = this->access_manager->get( pending.request ); }
        if( pending.operation == QNetworkAccessManager::PostOperation ){ ref_reply = this->access_manager->post( pending.request , &data ); }
        if( pending.operation == QNetworkAccessManager::PutOperation ){ ref_reply = this->access_manager->put( pending.request , &data ); }
        if( pending.operation == QNetworkAccessManager::DeleteOperation ){ ref_reply = this->access_manager->deleteResource( pending.request ); }
        if( pending.operation == QNetworkAccessManager::CustomOperation ){ ref_reply = this->access_manager->sendCustomRequest( pending.request , "" , &data ); }

        if( ref_reply ){

            this->mutex.lockForWrite();
            this->current_requests_amount++;
            this->mutex.unlock();

            ref_reply->connect( ref_reply , &QNetworkReply::finished , [ this ](){

                this->mutex.lockForWrite();
                this->current_requests_amount--;
                this->mutex.unlock();

                this->mutex.lockForRead();
                if( !this->pending_requests.isEmpty() ){
                    this->mutex.unlock();

                    this->mutex.lockForWrite();
                    GWSAPIDriverElement next = this->pending_requests.at( 0 );
                    this->pending_requests.removeAt( 0 );
                    this->mutex.unlock();

                    QtConcurrent::run([this , &next] {
                        this->executePendingOperation( next );
                    });

                } else {
                    this->mutex.unlock();
                }
            });
        }

        pending.callback( ref_reply );

    } catch(...){
        qWarning() << QString("Crashed request %1 %2").arg( pending.operation ).arg( pending.request.url().toString() );
    }

}

