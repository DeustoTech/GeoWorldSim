#include "Environment.h"

#include <QJsonObject>

QString geoworldsim::environment::Environment::SKIP_INDEXING = "skip_indexing";
QString geoworldsim::environment::Environment::SKIP_SUBSCRIBING = "skip_subscribing";

geoworldsim::environment::Environment::Environment() : Entity(){
}

geoworldsim::environment::Environment::~Environment(){
}

/**********************************************************************
 AGENTS METHODS
**********************************************************************/

void geoworldsim::environment::Environment::deserialize(const QJsonObject &json, QSharedPointer<Object> parent){
    // Avoid GWSEntity deserialisation
    Object::deserialize( json , parent );
}

QJsonObject geoworldsim::environment::Environment::serialize() const{
    return Object::serialize(); // Jump all agent serialisation
}

void geoworldsim::environment::Environment::registerEntity( QSharedPointer<Entity> entity){
    if( entity->environments_registerd_in.contains( this ) ){
        return;
    }
    entity->environments_registerd_in.append( this );
}

void geoworldsim::environment::Environment::unregisterEntity( QSharedPointer<Entity> entity){
    entity->environments_registerd_in.removeAll( this );
}
