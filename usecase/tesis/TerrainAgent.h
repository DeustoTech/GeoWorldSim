#ifndef TERRAINAGENT_H
#define TERRAINAGENT_H

#include "../../agent/Agent.h"

class TerrainAgent : public GWSAgent
{
    Q_OBJECT // Needed macro

    virtual void behave();

public:
    TerrainAgent();
};

#endif // TERRAINAGENT_H
