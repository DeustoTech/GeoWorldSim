#ifndef GWSGRIDENVIRONMENT_H
#define GWSGRIDENVIRONMENT_H

#include "../../environment/Environment.h"

#include "../../agent/Agent.h"
#include "../../util/storage/ObjectStorage.h"
#include "../../util/grid/Grid.h"

class GWSGridEnvironment : public GWSEnvironment
{
    Q_OBJECT

public:
    static GWSGridEnvironment* globalInstance();

    // GETTERS


    // METHODS
    virtual void registerAgent(GWSAgent *agent);
    virtual void unregisterAgent(GWSAgent *agent);

private:
    GWSGridEnvironment();
    GWSGridEnvironment(GWSGridEnvironment const&);
    ~GWSGridEnvironment();


    GWSObjectStorage* environment_grids; // Grids
};

#endif // GWSGRIDENVIRONMENT_H
