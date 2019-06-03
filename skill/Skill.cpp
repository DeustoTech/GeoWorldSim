#include "Skill.h"

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
    QJsonValue property_value = GWSObject::getProperty( name );
    QString property_value_as_string = property_value.toString();

    // If it comes between '<>', it is not the property name, but a key to fetch that property from the entity
    if( property_value_as_string.startsWith("<") && property_value_as_string.endsWith(">") ){
        QSharedPointer<GWSEntity> entity = this->getEntity();
        QString property_name = property_value_as_string.remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        return entity->getProperty( property_name );
    } else {
        return property_value;
    }
}
