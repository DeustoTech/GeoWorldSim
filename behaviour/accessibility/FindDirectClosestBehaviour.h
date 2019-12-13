#ifndef FINDDIRECTCLOSESTBEHAVIOUR_H
#define FINDDIRECTCLOSESTBEHAVIOUR_H

#include "../Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class FindDirectClosestBehaviour : public Behaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit FindDirectClosestBehaviour();
    ~FindDirectClosestBehaviour();

    // PARAMETERS:
    static QString ENTITY_TYPE_TO_FIND;
    static QString STORE_DIRECT_CLOSEST_ID_AS;
    static QString STORE_DIRECT_CLOSEST_GEOM_AS;
    static QString STORE_DIRECT_DISTANCE_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_DIRECT_CLOSEST_FOUND;

protected:
     virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // FINDDIRECTCLOSESTBEHAVIOUR_H
