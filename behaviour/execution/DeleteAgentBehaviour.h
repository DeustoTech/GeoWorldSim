#ifndef DELETEAGENTBEHAVIOUR_H
#define DELETEAGENTBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DeleteAgentBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit DeleteAgentBehaviour();


private slots:
    virtual QPair< double , QJsonArray > behave();



};

#endif // DELETEAGENTBEHAVIOUR_H
