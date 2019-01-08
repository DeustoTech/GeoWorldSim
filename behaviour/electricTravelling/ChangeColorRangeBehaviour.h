#ifndef CHANGECOLORBEHAVIOUR_H
#define CHANGECOLORBEHAVIOUR_H

#include <QObject>
#include "../../behaviour/Behaviour.h"

class ChangeColorRangeBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit ChangeColorRangeBehaviour();

    // PARAMETERS
    static QString MIN_EDGE_CAPACITY;
    static QString MAX_EDGE_CAPACITY;
    static QString NEXTS;

protected slots:

    virtual QJsonArray behave();

};

#endif // CHANGECOLORBEHAVIOUR_H
