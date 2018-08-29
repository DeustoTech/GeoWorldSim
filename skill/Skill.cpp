#include "Skill.h"

GWSSkill::GWSSkill() : GWSObject(){
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

QSharedPointer<GWSAgent> GWSSkill::getAgent(){
    return this->getParent().dynamicCast<GWSAgent>();
}
