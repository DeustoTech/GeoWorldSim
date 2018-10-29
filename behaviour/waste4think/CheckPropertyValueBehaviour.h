#ifndef CHECKPROPERTYVALUEBEHAVIOUR_H
#define CHECKPROPERTYVALUEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CheckPropertyValueBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit CheckPropertyValueBehaviour();

    // PROPERTIES

    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;
    static QString PROPERTY_TO_COMPARE_NAME;
    static QString PROPERTY_TO_COMPARE_THRESHOLD_VALUE;


protected slots:
    virtual QStringList behave();

};

#endif // CHECKPROPERTYVALUEBEHAVIOUR_H
