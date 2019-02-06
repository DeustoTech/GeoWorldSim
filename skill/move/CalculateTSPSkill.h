#ifndef CALCULATETSPSKILL_H
#define CALCULATETSPSKILL_H

#include "../Skill.h"

class CalculateTSPSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CalculateTSPSkill();

    // PROPERTIES
    static QString AGENT_PENDING_TSP_ROUTE_ELEMENTS;

};

#endif // CALCULATETSPSKILL_H
