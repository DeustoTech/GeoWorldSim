#ifndef GWSCOMMUNICATIONENVIRONMENT_H
#define GWSCOMMUNICATIONENVIRONMENT_H

#include "../../environment/Environment.h"
#include "../../util/network/ListenerWebSocket.h"
#include "../../util/network/PublisherWebSocket.h"
#include "../../app/App.h"

namespace geoworldsim {
namespace environment {

class CommunicationEnvironment : public Environment
{
    Q_OBJECT

public:
    static CommunicationEnvironment* globalInstance();

    // METHODS
    virtual void registerEntity( QSharedPointer<Entity> agent );
    virtual void unregisterEntity( QSharedPointer<Entity> agent );

signals: // MUST BE USED THROUGH THESE SIGNALS FOR ASYNC INVOKING
    void listenSocketSignal( QString socket_id );
    void sendEntitySignal( const QJsonObject &entity_json , const QString &socket_id = App::globalInstance()->getAppId() );
    void sendMessageSignal( const QJsonObject &message_json , const QString &socket_id = App::globalInstance()->getAppId() );
    void dataReceivedSignal( const QJsonObject &message_json , const QString &socket_id );

private slots:
    void dataReceivedFromSocket( QJsonObject data );

    // SOCKETS
    void connectExternalSocket( const QString &socket_id );
    void disconnectExternalSocket( const QString &socket_id );

    // SOCKET DATA
    void sendEntity( const QJsonObject &entity_json , const QString &socket_id = App::globalInstance()->getAppId() );
    void sendMessage( const QJsonObject &message_json , const QString &socket_id = App::globalInstance()->getAppId() );
    void sendData( const QString &signal , const QJsonObject &data , const QString &socket_id = App::globalInstance()->getAppId() );

private:
    CommunicationEnvironment();
    CommunicationEnvironment(CommunicationEnvironment const&);
    ~CommunicationEnvironment();

    QMap<QString , network::ListenerWebSocket*> listeners;
    QMap<QString , network::PublisherWebSocket*> publishers;

};

}
}

#endif // GWSCOMMUNICATIONENVIRONMENT_H
