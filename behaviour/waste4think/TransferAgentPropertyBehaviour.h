#ifndef TRANSFERAGENTPROPERTYBEHAVIOUR_H
#define TRANSFERAGENTPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class TransferAgentPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit TransferAgentPropertyBehaviour();

    // PROPERTIES
    static QString PROPERTY_TO_TRANSFER;
    static QString RECEIVING_AGENT_ID;
    static QString ID_FROM_PROPERTY;
    static QString NEXTS;

protected slots:
    virtual QStringList behave();
};

#endif // TRANSFERAGENTPROPERTYBEHAVIOUR_H
