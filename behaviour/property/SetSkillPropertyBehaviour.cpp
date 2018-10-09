#include "SetSkillPropertyBehaviour.h"

QString SetSkillPropertyBehaviour::SKILL_TYPE = "skill";
QString SetSkillPropertyBehaviour::SKILL_PROPERTY_NAME = "skill_property_name";
QString SetSkillPropertyBehaviour::SKILL_PROPERTY_VALUE = "skill_property_value";

SetSkillPropertyBehaviour::SetSkillPropertyBehaviour() : GWSBehaviour(){
}


bool SetSkillPropertyBehaviour::continueToNext(){

    /*QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSSkill> skill_to_modify = agent->getSkill( SKILL_TYPE ).dynamicCast<GWSSkill>();
    QString skill_to_modify_property = skill_to_modify->getProperty( SKILL_PROPERTY_NAME );

    // Unless the value of the property equals the behaviour's input value
    if ( skill_to_modify_property != SetSkillPropertyBehaviour::SKILL_PROPERTY_VALUE ){
         return false;
    }

    return true;
*/
}

bool SetSkillPropertyBehaviour::behave(){

    /*QSharedPointer<GWSAgent> agent = this->getAgent();
    QSharedPointer<GWSSkill> skill_to_modify = agent->getSkill( SetSkillPropertyBehaviour::SKILL_TYPE );
    skill_to_modify->setProperty( SetSkillPropertyBehaviour::SKILL_PROPERTY_NAME , SetSkillPropertyBehaviour::SKILL_PROPERTY_VALUE );
agent->getSkill( SetSkillPropertyBehaviour::SKILL_TYPE ).
    return true;
*/
}
