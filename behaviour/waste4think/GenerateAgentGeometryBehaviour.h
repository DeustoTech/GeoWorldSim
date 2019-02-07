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
    static QString SET_X_VALUE;
    static QString SET_Y_VALUE;
    static QString SET_JSON_VALUE;

protected slots:
    virtual QJsonArray behave();
};

#endif // GENERATEAGENTGEOMETRYBEHAVIOUR_H
