#ifndef HUMANAGENT_H
#define HUMANAGENT_H

#include "../../entity/Entity.h"

class HumanAgent : public GWSEntity
{
    Q_OBJECT

public:

    // Constructor
    Q_INVOKABLE HumanAgent();
    // METHODS
    ~HumanAgent();

};

Q_DECLARE_METATYPE(HumanAgent*) // REQUIRED IN EVERY CHILD

#endif // HUMANAGENT_H
