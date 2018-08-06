#ifndef TERRAINAGENT_H
#define TERRAINAGENT_H

#include "../../agent/Agent.h"
#include "../../util/grid/AgentGrid.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"

class TerrainAgent : public GWSAgent , public GWSAgentGrid
{
    Q_OBJECT // Needed macro

public:

    // CONSTRUCTOR
    Q_INVOKABLE explicit TerrainAgent(QObject* parent = Q_NULLPTR );

    // IMPORTERS
    void deserialize(QJsonObject json);
    virtual QJsonObject serialize() const;

    // METHODS
    virtual void behave();
};

Q_DECLARE_METATYPE(TerrainAgent*) // REQUIRED IN EVERY CHILD

#endif // TERRAINAGENT_H
