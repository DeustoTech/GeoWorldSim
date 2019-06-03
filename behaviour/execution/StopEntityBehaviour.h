#ifndef STOPENTITYBEHAVIOUR_H
#define STOPENTITYBEHAVIOUR_H

#include "../Behaviour.h"

class StopEntityBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StopEntityBehaviour();

private slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // STOPENTITYBEHAVIOUR_H
