#ifndef DETERMINEAGENTTYPEBEHAVIOUR_H
#define DETERMINEAGENTTYPEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DetermineAgentTypeBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:

    Q_INVOKABLE explicit DetermineAgentTypeBehaviour();

    virtual bool behave();
    //virtual bool continueToNext();

private:
    GWSResourceUnit input_resource;

};

#endif // DETERMINEAGENTTYPEBEHAVIOUR_H
