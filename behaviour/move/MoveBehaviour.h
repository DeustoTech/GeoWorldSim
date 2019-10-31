#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {


class MoveBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit MoveBehaviour();

    // PROPERTIES
    static QString INPUT_MOVE_TO;
    static QString NEXTS_IF_ARRIVED;
    static QString NEXTS_IF_NOT_ARRIVED;

protected slots:
    virtual QPair< double , QJsonArray > behave();

protected:
    virtual void afterCreateHook();
};

}
}

#endif // MOVEBEHAVIOUR_H

