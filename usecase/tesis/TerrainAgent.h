#ifndef TERRAINAGENT_H
#define TERRAINAGENT_H

#include "../../agent/Agent.h"
#include "../../util/grid/Grid.h"

class TerrainAgent : public GWSAgent, public GWSGrid
{
    Q_OBJECT // Needed macro

    virtual void behave();

public:
    TerrainAgent();
};

#endif // TERRAINAGENT_H
