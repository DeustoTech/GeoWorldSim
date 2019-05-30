#ifndef CHECKIFAGENTSRUNNINGBEHAVIOUR_H
#define CHECKIFAGENTSRUNNINGBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CheckIfAgentsRunningBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfAgentsRunningBehaviour();

    // PROPERTIES
    static QString AGENTS_TYPE;
    static QString AGENTS_FILTER;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;

private slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // CHECKIFAGENTSRUNNINGBEHAVIOUR_H
