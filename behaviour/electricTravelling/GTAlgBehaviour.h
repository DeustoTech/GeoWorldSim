#ifndef GTALGBEHAVIOUR_H
#define GTALGBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GTAlgBehaviour : public GWSBehaviour
{
     Q_OBJECT


public:
    Q_INVOKABLE explicit GTAlgBehaviour();


    // PROPERTIES
    static QString TRIP_LEGS;
    static QString NEXT_TRIP_LEG;
    static QString NEXTS_IF_LEGS;
    static QString NEXTS_IF_NO_LEGS;


private slots:
    virtual QPair< double , QJsonArray > behave();
    void dataReceived();

};

#endif // GTALGBEHAVIOUR_H
