#ifndef GWSCOMMUNICATIONENVIRONMENT_H
#define GWSCOMMUNICATIONENVIRONMENT_H

#include "../../environment/Environment.h"
#include "../../util/distributed/ExternalListener.h"
#include "../../util/distributed/ExternalPublisher.h"
#include "../../app/App.h"

class GWSCommunicationEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSCommunicationEnvironment* globalInstance();

    // METHODS
    virtual void registerEntity( QSharedPointer<GWSEntity> agent );
    virtual void unregisterEntity( QSharedPointer<GWSEntity> agent );

signals: // MUST BE USED THROUGH THESE SIGNALS FOR ASYNC INVOKING
    void listenSocketSignal( QString socket_id );
    void sendAgentSignal( const QJsonObject &agent_json , const QString &socket_id = GWSApp::globalInstance()->getAppId() );
    void sendMessageSignal( const QJsonObject &message_json , const QString &socket_id = GWSApp::globalInstance()->getAppId() );
    void dataReceivedSignal( const QJsonObject &message_json , const QString &socket_id );

private slots:
    void dataReceivedFromSocket( QJsonObject data );

    // SOCKETS
    void connectExternalSocket( const QString &socket_id );
    void disconnectExternalSocket( const QString &socket_id );

    // SOCKET DATA
    void sendAgent( const QJsonObject &agent_json , const QString &socket_id = GWSApp::globalInstance()->getAppId() );
    void sendMessage( const QJsonObject &message_json , const QString &socket_id = GWSApp::globalInstance()->getAppId() );
    void sendData( const QString &signal , const QJsonObject &data , const QString &socket_id = GWSApp::globalInstance()->getAppId() );

private:
    GWSCommunicationEnvironment();
    GWSCommunicationEnvironment(GWSCommunicationEnvironment const&);
    ~GWSCommunicationEnvironment();

    QMap<QString , GWSExternalListener*> listeners;
    QMap<QString , GWSExternalPublisher*> publishers;

};

#endif // GWSCOMMUNICATIONENVIRONMENT_H
