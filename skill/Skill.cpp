#include "Skill.h"

GWSSkill::GWSSkill() : GWSObject(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSSkill::deserialize(QJsonObject json, QSharedPointer<GWSObject> skilled_agent){
    this->skilled_agent = skilled_agent.dynamicCast<GWSAgent>();
    GWSObject::deserialize( json , skilled_agent );
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
    return this->skilled_agent;
}
