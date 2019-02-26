#ifndef CREATECHILDBEHAVIOUR_H
#define CREATECHILDBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CreateChildBehaviour : GWSBehaviour
{
    Q_OBJECT;

public:

    Q_INVOKABLE explicit CreateChildBehaviour();

    // PARAMETERS:
    static QString NEXTS;

protected slots:

    virtual QJsonArray behave();

};

#endif // CREATECHILDBEHAVIOUR_H
