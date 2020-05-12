#ifndef CALCULATEGTALGROUTEBEHAVIOUR_H
#define CALCULATEGTALGROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class CalculateGTAlgRouteBehaviour : public Behaviour
{
     Q_OBJECT

public:

    Q_INVOKABLE explicit CalculateGTAlgRouteBehaviour();

    // PROPERTIES
    static QString GTALG_HOST;
    static QString DESTINATION_X;
    static QString DESTINATION_Y;
    static QString DESTINATION_JSON;
    static QString TRANSPORT_MODE;
    static QString OPTIMIZATION;
    static QString STOP_ENTITY_IF_NO_ROUTE;

    static QString NEXTS;

private slots:
    virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // CALCULATEGTALGROUTEBEHAVIOUR_H
