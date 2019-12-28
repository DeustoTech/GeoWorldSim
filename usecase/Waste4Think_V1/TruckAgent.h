#ifndef TRUCKAGENT_H
#define TRUCKAGENT_H

#include "../../agent/Agent.h"

class TruckAgent : public GWSAgent
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE TruckAgent();

    // METHODS
    ~TruckAgent();
};

Q_DECLARE_METATYPE(TruckAgent*) // REQUIRED IN EVERY CHILD


#endif // TRUCKAGENT_H
