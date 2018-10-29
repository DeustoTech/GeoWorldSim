#ifndef GENERATEAGENTGEOMETRYBEHAVIOUR_H
#define GENERATEAGENTGEOMETRYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GenerateAgentGeometryBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GenerateAgentGeometryBehaviour();

    // PROPERTIES:
    static QString NEXTS;
    static QString X_VALUE;
    static QString Y_VALUE;

protected slots:
    virtual QStringList behave();
};

#endif // GENERATEAGENTGEOMETRYBEHAVIOUR_H
