#ifndef COMPOSTINGFACILITYAGENT_H
#define COMPOSTINGFACILITYAGENT_H

#include "../../agent/Agent.h"


class CompostingFacilityAgent : public GWSAgent
{
    Q_OBJECT

public:
    // Constructor
    Q_INVOKABLE CompostingFacilityAgent();

    // METHODS
    ~CompostingFacilityAgent();
};

Q_DECLARE_METATYPE(CompostingFacilityAgent*) // REQUIRED IN EVERY CHILD


#endif // COMPOSTINGFACILITYAGENT_H
