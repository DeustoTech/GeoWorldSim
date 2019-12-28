#ifndef TREEFARMAGENT_H
#define TREEFARMAGENT_H

#include "../../agent/Agent.h"

class TreeFarmAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE TreeFarmAgent();

    // METHODS
    ~TreeFarmAgent();
};

Q_DECLARE_METATYPE(TreeFarmAgent*) // REQUIRED IN EVERY CHILD

#endif // TREEFARMAGENT_H
