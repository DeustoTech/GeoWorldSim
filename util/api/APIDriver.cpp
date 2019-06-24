#include "APIDriver.h"

#include <QDebug>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QThreadPool>

#include "../../app/App.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"

GWSAPIDriver* GWSAPIDriver::globalInstance(){
    static GWSAPIDriver instance;
    return &instance;
}

GWSAPIDriver::GWSAPIDriver() : QObject(){
    this->pending_requests = new QList< GWSAPIDriverElement* >();
}

GWSAPIDriver::GWSAPIDriver(const GWSAPIDriver &other) : QObject(){
    Q_UNUSED( other );
}

GWSAPIDriver::~GWSAPIDriver(){
    for(int i = 0 ; i < this->pending_requests->size() ; i++) {
        delete this->pending_requests->at( i );
    }
    delete this->pending_requests;
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

    GWSAPIDriverElement* pending_request = new GWSAPIDriverElement();

    // Request
    pending_request->request = QNetworkRequest(url);
    QSslConfiguration sslConfiguration( QSslConfiguration::defaultConfiguration() );
    sslConfiguration.setProtocol( QSsl::TlsV1_2 );
    pending_request->request.setSslConfiguration( sslConfiguration );


    // Operation
    pending_request->operation = operation;

    // Add request headers
    foreach ( const QString header, headers.keys() ){
        pending_request->request.setRawHeader( header.toStdString().c_str(), headers.value( header ).toStdString().c_str() );
    }

    // Add data
    pending_request->data = data;

    // Add callback
    pending_request->callback = callback;

    //this->mutex.lockForRead();
    if( this->current_requests_amount > 6 ){ // Magic number set by Qt : https://doc.qt.io/qt-5/qnetworkaccessmanager.html
        //this->mutex.unlock();

        //this->mutex.lockForWrite();
        this->pending_requests->append( pending_request );
        //this->mutex.unlock();

    } else {
        //this->mutex.unlock();
        this->executePendingOperation( pending_request );
    }

}

void GWSAPIDriver::executePendingOperation( GWSAPIDriverElement* pending ){

    try {

        //this->mutex.lockForWrite();
        this->current_requests_amount++;
        //this->mutex.unlock();

        QTimer::singleShot( 10 , GWSApp::globalInstance() , [ pending , this ]{

            if( !this->access_manager ){
                QNetworkProxyQuery npq( QUrl( "https://google.com" ) );
                QList<QNetworkProxy> proxies_list = QNetworkProxyFactory::systemProxyForQuery( npq );

                if( proxies_list.isEmpty() ){
                    qWarning() << Q_FUNC_INFO << "No proxy found.";
                }

                // Must be created in main thread for it to work
                this->access_manager = new QNetworkAccessManager();
                this->access_manager->setProxy( proxies_list.at(0) );
            }

            QNetworkReply* ref_reply = Q_NULLPTR;
            QBuffer data;
            data.open( ( QBuffer::ReadWrite ) );
            data.write( pending->data );
            data.seek( 0 );

            if( pending->operation == QNetworkAccessManager::GetOperation ){ ref_reply = this->access_manager->get( pending->request ); }
            if( pending->operation == QNetworkAccessManager::PostOperation ){ ref_reply = this->access_manager->post( pending->request , &data ); }
            if( pending->operation == QNetworkAccessManager::PutOperation ){ ref_reply = this->access_manager->put( pending->request , &data ); }
            if( pending->operation == QNetworkAccessManager::DeleteOperation ){ ref_reply = this->access_manager->deleteResource( pending->request ); }
            if( pending->operation == QNetworkAccessManager::CustomOperation ){ ref_reply = this->access_manager->sendCustomRequest( pending->request , "" , &data ); }

            if( ref_reply ){

                ref_reply->connect( ref_reply , &QNetworkReply::finished , [ this ](){

                    this->current_requests_amount--;

                    //this->mutex.lockForRead();
                    if( !this->pending_requests->isEmpty() ){
                        //this->mutex.unlock();

                        //this->mutex.lockForWrite();
                        GWSAPIDriverElement* next = this->pending_requests->at( 0 );
                        this->pending_requests->removeAt( 0 );
                        //this->mutex.unlock();

                        this->executePendingOperation( next );

                    } else {
                        //this->mutex.unlock();
                    }
                });

            } else {

                this->current_requests_amount--;
            }

            pending->callback( ref_reply );
            delete pending;

        });

    } catch(...){
        qWarning() << QString("Crashed request %1 %2").arg( pending->operation ).arg( pending->request.url().toString() );
    }

}

