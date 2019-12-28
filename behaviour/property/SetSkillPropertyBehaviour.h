#ifndef SETSKILLPROPERTYBEHAVIOUR_H
#define SETSKILLPROPERTYBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class SetSkillPropertyBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit SetSkillPropertyBehaviour();

    virtual bool canContinueToNext();

    //PROPERTIES
    static QString SKILL_TYPE;
    static QString SKILL_PROPERTY_NAME;
    static QString SKILL_PROPERTY_VALUE;

protected:
    virtual bool behave();
};

#endif // SETSKILLPROPERTYBEHAVIOUR_H
