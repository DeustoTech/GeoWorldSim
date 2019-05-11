#ifndef TRANSACTIONBEHAVIOUR_H
#define TRANSACTIONBEHAVIOUR_H

#include <QJsonValue>

#include "../../behaviour/Behaviour.h"

class TransactionBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit TransactionBehaviour();

    // PROPERTIES
    static QString TRANSACTION_TYPE;
    static QString TRANSACTION_DATA;
    static QString EMITTING_AGENT_ID;
    static QString RECEIVING_AGENT_ID;
    static QString NEXTS;

protected slots:
     virtual QPair< double , QJsonArray > behave();

private:
    QJsonValue incrementQJsonValue( QJsonValue existing_value , QJsonValue increment );
};

#endif // TRANSFERAGENTPROPERTYBEHAVIOUR_H
