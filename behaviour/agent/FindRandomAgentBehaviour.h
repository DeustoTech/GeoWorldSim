#ifndef FINDRANDOMAGENTBEHAVIOUR_H
#define FINDRANDOMAGENTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class FindRandomAgentBehaviour : GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindRandomAgentBehaviour();

    // PARAMETERS:
    static QString NEXTS_IF_FOUND;
    static QString NEXTS_IF_NOT_FOUND;
    static QString RANDOM_AGENT_TYPE;
    static QString STORE_RANDOM_AGENT_AS;
    static QString STORE_RANDOM_AGENT_X_AS;
    static QString STORE_RANDOM_AGENT_Y_AS;


protected slots:
    virtual QJsonArray behave();
};

#endif // FINDRANDOMAGENTBEHAVIOUR_H
