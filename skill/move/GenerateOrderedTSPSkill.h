#ifndef GENERATETSPSKILL_H
#define GENERATETSPSKILL_H

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../util/random/UniformDistribution.h"
#include "../../util/routing/TSPRouting.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"

#include "../../object/ObjectFactory.h"

class GenerateOrderedTSPSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GenerateOrderedTSPSkill();

    // PROPERTIES
    static QString VISIT_AGENTS_PROP;

    // METHODS
    QList< QPair< GWSCoordinate , QString > > generateOrderedTSP();
    virtual void generateGraph();

protected:

    QList< QPair< GWSCoordinate , QString > >  ordered_agents_to_visit_tsp_route_coord_id_array;

};

#endif // GENERATETSPSKILL_H
