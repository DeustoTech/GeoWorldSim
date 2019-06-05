#include "Skill.h"

#include "../../object/ObjectFactory.h"

GWSSkill::GWSSkill() : GWSObject(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSSkill::deserialize(QJsonObject json, QSharedPointer<GWSObject> skilled_entity){
    this->skilled_entity = skilled_entity.dynamicCast<GWSEntity>();
    GWSObject::deserialize( json , skilled_entity );
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

QSharedPointer<GWSEntity> GWSSkill::getEntity() const{
    return this->skilled_entity;
}

const QJsonValue GWSSkill::getProperty( QString name ) const{
    return GWSObjectFactory::simpleOrParentPropertyValue( name , this->getSharedPointer() , this->getEntity() );
}
