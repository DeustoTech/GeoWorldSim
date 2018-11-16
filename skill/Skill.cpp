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

QSharedPointer<GWSAgent> GWSSkill::getAgent() const{
    return this->skilled_agent;
}

const QJsonValue GWSSkill::getProperty( QString name ) const{
    QJsonValue property_value = GWSObject::getProperty( name );
    QString property_value_as_string = property_value.toString();

    // If it comes between '<>', it is not the property name, but a key to fetch that property from the agent
    if( property_value_as_string.startsWith("<") && property_value_as_string.endsWith(">") ){
        QSharedPointer<GWSAgent> agent = this->getAgent();
        QString property_name = property_value_as_string.remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        return agent->getProperty( property_name );
    } else {
        return property_value;
    }
}
