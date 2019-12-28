#ifndef TIMBERFACTORYAGENT_H
#define TIMBERFACTORYAGENT_H

#include "../../agent/Agent.h"


class TimberFactoryAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE TimberFactoryAgent();

    // METHODS
    ~TimberFactoryAgent();
};

Q_DECLARE_METATYPE(TimberFactoryAgent*) // REQUIRED IN EVERY CHILD
#endif // TIMBERFACTORYAGENT_H
