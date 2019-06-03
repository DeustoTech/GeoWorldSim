#ifndef CHECKIFENTITIESRUNNINGBEHAVIOUR_H
#define CHECKIFENTITIESRUNNINGBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CheckIfEntitiesRunningBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfEntitiesRunningBehaviour();

    // PROPERTIES
    static QString ENTITY_TYPE;
    static QString ENTITY_FILTER;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

private slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // CHECKIFENTITIESRUNNINGBEHAVIOUR_H
