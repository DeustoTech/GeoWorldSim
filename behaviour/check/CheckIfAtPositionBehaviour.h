#ifndef CHECKIFATPOSITION_H
#define CHECKIFATPOSITION_H

#include "../../object/ObjectFactory.h"
#include "../../behaviour/Behaviour.h"

class CheckIfAtPositionBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfAtPositionBehaviour();

    // PROPERTIES
    static QString POSITION_X_VALUE;
    static QString POSITION_Y_VALUE;
    static QString NEXTS_IF_AT_POSITION;
    static QString NEXTS_IF_NOT_AT_POSITION;


private slots:
    virtual QStringList behave();
};

#endif // CHECKIFATPOSITION_H
