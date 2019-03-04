#ifndef DETERMINEACCESIBILITYBEHAVIOUR_H
#define DETERMINEACCESIBILITYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DetermineAccesibilityBehaviour : GWSBehaviour
{
    Q_OBJECT

public:
    explicit Q_INVOKABLE DetermineAccesibilityBehaviour();

    // PARAMETERS
    static QString FACILITY_TO_ACCESS;
    static QString MAXIMUM_ACCESSIBILITY_DISTANCE;
    static QString NETWORK_TO_ACCESS;
    static QString NEXT;

protected slots:

    virtual QJsonArray behave();
};

#endif // DETERMINEACCESIBILITYBEHAVIOUR_H
