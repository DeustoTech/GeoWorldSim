#ifndef GTALGBEHAVIOUR_H
#define GTALGBEHAVIOUR_H


#include "../../behaviour/Behaviour.h"


class GTAlgBehaviour : public GWSBehaviour
{
     Q_OBJECT


public:
    Q_INVOKABLE explicit GTAlgBehaviour();


    // PROPERTIES
    static QString VEHICLE_TYPE;
    static QString STORE_NOISE_AS;
    static QString NEXTS;


private slots:
    virtual QPair< double , QJsonArray > behave();

};

#endif // GTALGBEHAVIOUR_H
