#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class MoveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveBehaviour();

    // PROPERTIES
    static QString INPUT_MOVE_TO;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;

private slots:
    virtual QPair< double , QJsonArray > behave();

};

#endif // MOVEBEHAVIOUR_H

