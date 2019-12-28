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
    static QString X_VALUE;
    static QString Y_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;


private slots:
    virtual QStringList behave();
};

#endif // CHECKIFATPOSITION_H
