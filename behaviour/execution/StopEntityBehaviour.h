#ifndef STOPENTITYBEHAVIOUR_H
#define STOPENTITYBEHAVIOUR_H

#include "../Behaviour.h"

namespace geoworldsim {
namespace behaviour {


class StopEntityBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StopEntityBehaviour();

private slots:
    virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // STOPENTITYBEHAVIOUR_H
