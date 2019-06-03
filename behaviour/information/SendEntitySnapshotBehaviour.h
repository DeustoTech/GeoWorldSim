#ifndef SENDENTITYSNAPSHOTBEHAVIOUR_H
#define SENDENTITYSNAPSHOTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"

class SendEntitySnapshotBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SendEntitySnapshotBehaviour();

    // PROPERTIES
    static QString ENTITY_TO_SEND_ID;
    static QString SOCKET_ID;
    static QString NEXTS;

private slots:

    virtual QPair< double , QJsonArray > behave();

};

#endif // SENDENTITYSNAPSHOTBEHAVIOUR_H
