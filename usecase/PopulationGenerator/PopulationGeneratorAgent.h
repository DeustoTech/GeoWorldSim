#ifndef POPULATIONGENERATORAGENT_H
#define POPULATIONGENERATORAGENT_H

#include "../../agent/Agent.h"

class PopulationGeneratorAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE PopulationGeneratorAgent();
    // METHODS
    ~PopulationGeneratorAgent();

};

Q_DECLARE_METATYPE(PopulationGeneratorAgent*) // REQUIRED IN EVERY CHILD

#endif // POPULATIONGENERATORAGENT_H
