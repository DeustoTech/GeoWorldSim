#ifndef GWSSKILL_H
#define GWSSKILL_H

#include "object/Object.h"
#include "agent/Agent.h"

class GWSSkill : public GWSObject
{
    Q_OBJECT
    friend class GWSAgent; // Agent can check skill insides

public:
    Q_INVOKABLE explicit GWSSkill( GWSAgent *skilled_agent = Q_NULLPTR );

    // GETTERS
    GWSAgent* getAgent();
};

#endif // GWSSKILL_H
