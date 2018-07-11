#include "Skill.h"

GWSSkill::GWSSkill( GWSAgent* skilled_agent ) : GWSObject( skilled_agent ){
}

GWSAgent* GWSSkill::getAgent(){
    return dynamic_cast<GWSAgent*>( this->parent() );
}
