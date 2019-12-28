#include "EntityEnvironment.h"

#include "../../environment/EnvironmentsGroup.h"

geoworldsim::environment::EntityEnvironment* geoworldsim::environment::EntityEnvironment::globalInstance(){
    static geoworldsim::environment::EntityEnvironment instance;
    return &instance;
}

geoworldsim::environment::EntityEnvironment::EntityEnvironment() : Environment(){
    this->environment_entities = new storage::ObjectStorage();
    this->environment_entities->setObjectName( QString("%1").arg( this->metaObject()->className() ) );
    qInfo() << "Agent environment created";
    EnvironmentsGroup::globalInstance()->addEnvironment( this );
}

geoworldsim::environment::EntityEnvironment::~EntityEnvironment(){
    qInfo() << "Agent environment deleted";
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

quint64 geoworldsim::environment::EntityEnvironment::getAmount() const{
    return this->environment_entities->getAmount();
}

bool geoworldsim::environment::EntityEnvironment::contains(QString class_name) const{
    return this->environment_entities && this->environment_entities->contains( class_name );
}

QSharedPointer<geoworldsim::Entity> geoworldsim::environment::EntityEnvironment::getRandomByClass(QString class_name){
    QList< QSharedPointer< Entity > > all = this->environment_entities->getByClass< Entity >( class_name );
    return all.at( qrand() % all.size() );
}

QSharedPointer<geoworldsim::Entity> geoworldsim::environment::EntityEnvironment::getByUID(QString id) const{
    return this->getByClassAndUID( geoworldsim::Entity::staticMetaObject.className() , id );
}

QList< QSharedPointer< geoworldsim::Entity > > geoworldsim::environment::EntityEnvironment::getByUIDS(QStringList ids) const{
    QList< QSharedPointer< Entity > > entities;
    foreach (QString id , ids) {
        entities.append( this->getByUID( id ) );
    }
    return entities;
}

QList< QSharedPointer<geoworldsim::Entity> > geoworldsim::environment::EntityEnvironment::getByUIDS(QJsonArray json) const{
    QStringList ids;
    foreach(QJsonValue v , json){
        ids.append( v.toString() );
    }
    return this->getByUIDS( ids );
}

QSharedPointer<geoworldsim::Entity> geoworldsim::environment::EntityEnvironment::getByClassAndUID( QString class_name , QString id) const{
    if( this->environment_entities ){
        QSharedPointer< Entity > obj = this->environment_entities->getByClassAndUID< Entity >( class_name , id );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<geoworldsim::Entity> geoworldsim::environment::EntityEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->environment_entities->getByClassAndName< Entity >( class_name , agent_name );
}

QList< QSharedPointer<geoworldsim::Entity> > geoworldsim::environment::EntityEnvironment::getByClass( QString class_name ) const{
    return this->environment_entities->getByClass< Entity >( class_name );
}

QList< QSharedPointer<geoworldsim::Entity> > geoworldsim::environment::EntityEnvironment::getMatches(QString class_name, QString uid) const{
    return this->environment_entities->getMatches< Entity >( class_name , uid );
}


QSharedPointer<geoworldsim::Entity> geoworldsim::environment::EntityEnvironment::getByName( QString agent_name ) const{
    return this->environment_entities->getByName< Entity >( agent_name );
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void geoworldsim::environment::EntityEnvironment::registerEntity( QSharedPointer< Entity > entity){
    Environment::registerEntity( entity );

    if( entity->getProperty( SKIP_INDEXING ).toBool() ){
        return;
    }

    emit this->environment_entities->addObjectSignal( entity );
}

void geoworldsim::environment::EntityEnvironment::unregisterEntity( QSharedPointer< Entity > entity){
    Environment::unregisterEntity( entity );
    emit this->environment_entities->removeObjectSignal( entity );
}

