#ifndef VIEWSKILL_H
#define VIEWSKILL_H

#include "../Skill.h"
#include "../../util/geometry/Geometry.h"

class ViewSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ViewSkill( GWSAgent *skilled_agent = Q_NULLPTR );

    // PROPERTIES
    static QString VIEWPORT_PROP; // Relative to agents' geometry
    static QString AGENT_TYPE_FIND_PROP;

    // GETTERS
    QList<GWSAgent*> getViewingAgents();

};

#endif // VIEWSKILL_H