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
    static QString EMITTING_ENTITY_ID;
    static QString RECEIVING_ENTITY_ID;
    static QString PROPERTY_NAMES_TO_TRANSFER;
    static QString NEXTS;

protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // TRANSFERAGENTPROPERTYBEHAVIOUR_H
