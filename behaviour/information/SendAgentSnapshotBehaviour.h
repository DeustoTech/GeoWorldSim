#ifndef BROADCASTTOHISTORYBEHAVIOUR_H
#define BROADCASTTOHISTORYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"

class SendAgentSnapshotBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SendAgentSnapshotBehaviour();

    // PARAMETERS
    static QString AGENT_TO_SEND_ID;
    static QString SOCKET_ID;
    static QString NEXTS;

private slots:

    virtual QPair< double , QJsonArray > behave();

};

#endif // BROADCASTTOHISTORYBEHAVIOUR_H
