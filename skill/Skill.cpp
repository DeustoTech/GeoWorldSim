#include "Skill.h"

GWSSkill::GWSSkill( GWSAgent* skilled_agent ) : GWSObject( skilled_agent ){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSSkill::deserialize(QJsonObject json){
    GWSObject::deserialize( json );
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

QJsonObject GWSSkill::serialize() const{
    return GWSObject::serialize();
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSAgent* GWSSkill::getAgent(){
    return dynamic_cast<GWSAgent*>( this->parent() );
}
