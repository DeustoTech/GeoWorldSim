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
    static QString CAPACITY_PROP;
    static QString CONTAINER_PROP;
    static QString NEXTS;

protected slots:

    virtual QJsonArray behave();

};

#endif // CHANGECOLORBEHAVIOUR_H
