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

    // If it comes between '<>', it is not the property name, but a kew to fetch said property name from one entities's value
    if( name.startsWith("<") && name.endsWith(">") ){
        QString property_key = name.remove( 0 , 1 );
        property_key = property_key.remove( property_key.length() - 1 , 1 );
        name = this->getEntity()->getProperty( property_key ).toString();
    }

    QJsonValue property_value = GWSObject::getProperty( name );
    QString property_value_as_string = property_value.toString();

    // If it comes between '<>', it is not the property name, but a key to fetch that property from the agent
    if( property_value_as_string.startsWith("<") && property_value_as_string.endsWith(">") ){
        QString property_name = property_value_as_string.remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        property_value = this->getEntity()->getProperty( property_name );
    }

    // PROPERTY IS OBJECT
    if( property_value.isObject() ){
        QJsonObject obj;
        foreach( QString key , property_value.toObject().keys() ) {

            QJsonValue obj_value = property_value.toObject().value( key );
            QString obj_value_as_string = obj_value.toString();

            // If it comes between '<>', it is not the property name, but a key to fetch that property from the agent
            if( obj_value_as_string.startsWith("<") && obj_value_as_string.endsWith(">") ){
                QString property_name = obj_value_as_string.remove( 0 , 1 );
                property_name = property_name.remove( property_name.length() - 1 , 1 );
                obj_value = this->getEntity()->getProperty( property_name );
            }

            obj.insert( key , obj_value );
        }
        property_value = obj;
    }

    return property_value;
}
