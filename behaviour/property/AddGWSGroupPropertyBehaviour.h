#ifndef ADDGWSGROUPPROPERTYBEHAVIOUR_H
#define ADDGWSGROUPPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class AddGWSGroupPropertyBehaviour : GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AddGWSGroupPropertyBehaviour();

    // PARAMETERS
    static QString PROPERTY_TO_ADD_TO_GWSGROUP;
    static QString NEXTS;

protected slots:

     virtual QPair< double , QJsonArray > behave();
};

#endif // ADDGWSGROUPPROPERTYBEHAVIOUR_H
