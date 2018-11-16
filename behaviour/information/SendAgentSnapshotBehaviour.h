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
    static QString NEXTS;
    static QString AGENT_TO_SEND_ID;

private slots:

    virtual QJsonArray behave();

};

#endif // BROADCASTTOHISTORYBEHAVIOUR_H
