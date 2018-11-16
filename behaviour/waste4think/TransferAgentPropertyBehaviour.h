#ifndef TRANSFERAGENTPROPERTYBEHAVIOUR_H
#define TRANSFERAGENTPROPERTYBEHAVIOUR_H

#include <QJsonValue>

#include "../../behaviour/Behaviour.h"

class TransferAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit TransferAgentPropertyBehaviour();

    // PROPERTIES
    static QString PROPERTY_NAME_TO_TRANSFER;
    static QString EMITTING_AGENT_ID;
    static QString RECEIVING_AGENT_ID;
    static QString NEXTS;
protected slots:
    virtual QStringList behave();

private:
    QJsonValue incrementQJsonValue( QJsonValue existing_value , QJsonValue increment );
};

#endif // TRANSFERAGENTPROPERTYBEHAVIOUR_H
