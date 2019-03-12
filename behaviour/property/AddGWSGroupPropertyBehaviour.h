#ifndef ADDGWSGROUPPROPERTYBEHAVIOUR_H
#define ADDGWSGROUPPROPERTYBEHAVIOUR_H


#include "../../behaviour/Behaviour.h"

class AddGWSGroupPropertyBehaviour : GWSBehaviour
{
    Q_OBJECT

public:
    explicit Q_INVOKABLE AddGWSGroupPropertyBehaviour();

    // PARAMETERS
    static QString PROPERTY_TO_ADD_TO_GWSGROUP;
    static QString NEXT;

protected slots:

    virtual QJsonArray behave();
};

#endif // ADDGWSGROUPPROPERTYBEHAVIOUR_H
