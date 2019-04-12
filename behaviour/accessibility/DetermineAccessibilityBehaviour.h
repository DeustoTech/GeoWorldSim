#ifndef DETERMINEACCESSIBILITYBEHAVIOUR_H
#define DETERMINEACCESSIBILITYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DetermineAccessibilityBehaviour : GWSBehaviour
{
    Q_OBJECT

public:
    explicit Q_INVOKABLE DetermineAccessibilityBehaviour();

    // PARAMETERS
    static QString FACILITY_TO_ACCESS;
    static QString MAXIMUM_ACCESSIBILITY_DISTANCE;
    static QString NETWORK_TO_ACCESS;
    static QString NEXT;

protected slots:

     virtual QPair< double , QJsonArray > behave();
};

#endif // DETERMINEACCESSIBILITYBEHAVIOUR_H
