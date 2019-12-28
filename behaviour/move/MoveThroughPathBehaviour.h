#ifndef MOVETHROUGHPATHBEHAVIOUR_H
#define MOVETHROUGHPATHBEHAVIOUR_H

#include "../../behaviour/move/MoveBehaviour.h"

namespace geoworldsim {
namespace behaviour {


class MoveThroughPathBehaviour : public MoveBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughPathBehaviour();

    // PROPERTIES
    static QString INPUT_ROUTE_PATH;

protected slots:
    virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // MOVETHROUGHPATHBEHAVIOUR_H
