#ifndef GTALGBEHAVIOUR_H
#define GTALGBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GTAlgBehaviour : public GWSBehaviour
{
     Q_OBJECT


public:
    Q_INVOKABLE explicit GTAlgBehaviour();


    // PROPERTIES
    static QString DESTINATION_X;
    static QString DESTINATION_Y;
    static QString TRANSPORT_MODE;
    static QString TRIP_LEGS;
    static QString CURRENT_LEG_MODE;
    static QString CURRENT_LEG_TO_X;
    static QString CURRENT_LEG_TO_Y;
    static QString CURRENT_LEG_DEPARTURE_TIME;
    static QString NEXTS_IF_LEGS;
    static QString NEXTS_IF_NO_LEGS;
    static QString NEXTS_IF_WAITING;


private slots:
    virtual QPair< double , QJsonArray > behave();
    void dataReceived();

};

#endif // GTALGBEHAVIOUR_H
