#include "SetSkillPropertyBehaviour.h"

#include "../../skill/Skill.h"

QString SetSkillPropertyBehaviour::SKILL_TYPE = "skill";
QString SetSkillPropertyBehaviour::SKILL_PROPERTY_NAME = "skill_property_name";
QString SetSkillPropertyBehaviour::SKILL_PROPERTY_VALUE = "skill_property_value";

SetSkillPropertyBehaviour::SetSkillPropertyBehaviour() : GWSBehaviour(){
}


bool SetSkillPropertyBehaviour::canContinueToNext(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString skill  = this->getProperty( SKILL_TYPE ).toString();
    QString property_name  = this->getProperty( SKILL_PROPERTY_NAME ).toString();
    QVariant property_value = this->getProperty( SKILL_PROPERTY_VALUE );
    QSharedPointer<GWSSkill> skill_to_modify = agent->getSkill( skill );

    // Unless the value of the property equals the behaviour's input value
    if ( this->getProperty( SKILL_PROPERTY_VALUE ) != skill_to_modify->getProperty( property_name ) ){
        return false;
    }

    return true;
}

bool SetSkillPropertyBehaviour::behave(){

    QString skill  = this->getProperty( SKILL_TYPE ).toString();
    QString property_name  = this->getProperty( SKILL_PROPERTY_NAME ).toString();
    QVariant property_value = this->getProperty( SKILL_PROPERTY_VALUE );

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QSharedPointer<GWSSkill> skill_to_modify = agent->getSkill( skill );
    skill_to_modify->setProperty( property_name, property_value );

return true;

}
