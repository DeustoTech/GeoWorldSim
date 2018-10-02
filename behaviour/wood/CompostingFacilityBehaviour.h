#ifndef COMPOSTINGFACILITYBEHAVIOUR_H
#define COMPOSTINGFACILITYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class CompostingFacilityBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit CompostingFacilityBehaviour();
    virtual bool behave();
};

#endif // COMPOSTINGFACILITYBEHAVIOUR_H
