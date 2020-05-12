#ifndef LISTENTOMESSAGESBEHAVIOUR_H
#define LISTENTOMESSAGESBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"
#include "../../app/App.h"
#include <QObject>

namespace geoworldsim {
namespace behaviour {

class ListenToMessagesBehaviour : public Behaviour
{
    Q_OBJECT


public:
    Q_INVOKABLE explicit ListenToMessagesBehaviour();

    // PARAMETERS
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();

private slots:
    void listen( QJsonObject message_json , QString socket_id );

public:
    QList< QJsonObject > received_pending_messages;
};

}
}

#endif // LISTENTOMESSAGESBEHAVIOUR_H
