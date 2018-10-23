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

private slots:

    virtual QStringList behave();

};

#endif // BROADCASTTOHISTORYBEHAVIOUR_H
