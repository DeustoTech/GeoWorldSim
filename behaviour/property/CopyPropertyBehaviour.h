#ifndef COPYPROPERTYBEHAVIOUR_H
#define COPYPROPERTYBEHAVIOUR_H

#include <QObject>

#include "../../behaviour/Behaviour.h"

class CopyPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit CopyPropertyBehaviour();

    // PROPERTIES
    static QString AGENT_ID_TO_COPY_FROM;
    static QString PROPERTY_NAME_TO_COPY;
    static QString NEXTS;

    virtual QJsonArray behave();
};

#endif // COPYPROPERTYBEHAVIOUR_H
