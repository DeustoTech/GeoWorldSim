#ifndef ASSIGNVEHICLESUBTYPEBEHAVIOUR_H
#define ASSIGNVEHICLESUBTYPEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour{

class AssignVehicleSubtypeBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AssignVehicleSubtypeBehaviour();

    // PROPERTIES
    static QString VEHICLE_TYPE;
    static QString VEHICLE_SUBTYPE;
    static QString DISTRIBUTION;
    static QString NEXTS;


private slots:
    virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // ASSIGNVEHICLESUBTYPEBEHAVIOUR_H
