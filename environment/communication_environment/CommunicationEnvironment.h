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
    void sendAgentSignal( QJsonObject agent_json , QString socket_id = GWSApp::globalInstance()->getAppId() );
    void sendMessageSignal( QJsonObject message_json , QString socket_id = GWSApp::globalInstance()->getAppId() );

private slots:
    // LISTENERS

    // PUBLISHERS
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
