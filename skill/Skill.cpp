#include "Skill.h"

#include "../../object/ObjectFactory.h"

geoworldsim::skill::Skill::Skill() : Object(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void geoworldsim::skill::Skill::deserialize(const QJsonObject &json, QSharedPointer<Object> skilled_entity){
    this->skilled_entity = skilled_entity.dynamicCast<geoworldsim::Entity>();
    Object::deserialize( json , skilled_entity );
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

QJsonObject geoworldsim::skill::Skill::serialize() const{
    return Object::serialize();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<geoworldsim::Entity> geoworldsim::skill::Skill::getEntity() const{
    return this->skilled_entity;
}

QJsonValue geoworldsim::skill::Skill::getProperty( const QString& name ) const{
    return ObjectFactory::simpleOrParentPropertyName( name , this->getSharedPointer() , this->getEntity() );
}
