#include "ExternalPublisher.h"

#include "../../app/App.h"
#include "../../object/Object.h"

#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>

GWSExternalPublisher::GWSExternalPublisher(const QString &socket_id) : GWSExternalCommunicator( socket_id){

}

void GWSExternalPublisher::sendMessage(const QString &signal , const QJsonValue &json){

    QJsonObject socket_json;
    socket_json.insert( "signal" , signal );
    socket_json.insert( "scenario_id" , this->socket_id );
    socket_json.insert( "body" , json );

    if( this->websocket.state() == QAbstractSocket::ConnectedState && this->communicator_status >= CONNECTED ){

        QTimer::singleShot( qrand() % 100 , this , [ socket_json , this ](){

            try {
                this->mutext.lockForRead();
                if( this->websocket.state() == QAbstractSocket::ConnectedState && this->communicator_status >= CONNECTED ){
                    this->mutext.unlock();

                    // TO sockets.geoworldsim.com
                    QByteArray arr = QJsonDocument( socket_json ).toJson( QJsonDocument::Compact );
                    this->mutext.lockForWrite();
                    this->websocket.sendBinaryMessage( arr );

                }
                this->mutext.unlock();
            } catch(...){}

        });
    }

}
