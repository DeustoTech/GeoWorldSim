#ifndef CALCULATEETPLANNERROUTEBEHAVIOUR_H
#define CALCULATEETPLANNERROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour{

class CalculateETPlannerRouteBehaviour : public Behaviour
{    
    Q_OBJECT

public:
     Q_INVOKABLE explicit CalculateETPlannerRouteBehaviour();

    // PROPERTIES
    static QString ETPLANNER_HOST;
    static QString DESTINATION_X;
    static QString DESTINATION_Y;
    static QString DESTINATION_JSON;
    static QString TRANSPORT_MODE;
    static QString COLOR;
    static QString TOTAL_DISTANCE;
    static QString SEGMENT_ID;
    static QString OPTIMIZATION;
    static QString IS_STATION;
    static QString STOP_ENTITY_IF_NO_ROUTE;

    static QString NEXTS;

private slots:
    virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // CALCULATEETPLANNERROUTEBEHAVIOUR_H
