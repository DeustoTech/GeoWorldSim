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
    virtual void registerAgent( QSharedPointer<GWSAgent> agent );
    virtual void unregisterAgent( QSharedPointer<GWSAgent> agent );

signals: // MUST BE USED THROUGH THESE SIGNALS FOR ASYNC INVOKING
    void listenSocket( QString socket_id );
    void sendAgentSignal( QJsonObject agent_json , QString socket_id = GWSApp::globalInstance()->getAppId() );
    void sendMessageSignal( QJsonObject message_json , QString socket_id = GWSApp::globalInstance()->getAppId() );
    void dataReceivedSignal( QJsonObject message_json , QString socket_id );


public slots:
    void dataReceivedFromSocket( QJsonObject data );


private slots:
    // SOCKETS
    void connectExternalSocket( QString socket_id );
    void disconnectExternalSocket( QString socket_id );

    // SOCKET DATA
    void sendAgent( QJsonObject agent_json , QString socket_id = GWSApp::globalInstance()->getAppId() );
    void sendMessage( QJsonObject message_json , QString socket_id = GWSApp::globalInstance()->getAppId() );
    void sendData( QString signal , QJsonObject data , QString socket_id = GWSApp::globalInstance()->getAppId() );

private:
    GWSCommunicationEnvironment();
    GWSCommunicationEnvironment(GWSCommunicationEnvironment const&);
    ~GWSCommunicationEnvironment();

    QMap<QString , GWSExternalListener*> listeners;
    QMap<QString , GWSExternalPublisher*> publishers;

};

#endif // GWSCOMMUNICATIONENVIRONMENT_H
