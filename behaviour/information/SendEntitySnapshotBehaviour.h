#ifndef SENDENTITYSNAPSHOTBEHAVIOUR_H
#define SENDENTITYSNAPSHOTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"

namespace geoworldsim {
namespace behaviour {

class SendEntitySnapshotBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SendEntitySnapshotBehaviour();

    // PROPERTIES
    static QString ENTITY_TO_SEND_UID;
    static QString SOCKET_ID;
    static QString NEXTS;

private slots:

    virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // SENDENTITYSNAPSHOTBEHAVIOUR_H
