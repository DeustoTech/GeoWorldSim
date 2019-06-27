#include "EntityEnvironment.h"

#include "../../environment/EnvironmentsGroup.h"

GWSEntityEnvironment* GWSEntityEnvironment::globalInstance(){
    static GWSEntityEnvironment instance;
    return &instance;
}

GWSEntityEnvironment::GWSEntityEnvironment() : GWSEnvironment(){
    this->environment_entities = new GWSObjectStorage();
    qInfo() << "Agent environment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSEntityEnvironment::~GWSEntityEnvironment(){
    qInfo() << "Agent environment deleted";
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

quint64 GWSEntityEnvironment::getAmount() const{
    return this->environment_entities->getAmount();
}

bool GWSEntityEnvironment::contains(QString class_name) const{
    return this->environment_entities && this->environment_entities->contains( class_name );
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getRandomByClass(QString class_name){
    QList< QSharedPointer<GWSEntity> > all = this->environment_entities->getByClass<GWSEntity>( class_name );
    return all.at( qrand() % all.size() );
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getByUID(QString id) const{
    return this->getByClassAndUID( GWSEntity::staticMetaObject.className() , id );
}

QList< QSharedPointer<GWSEntity> > GWSEntityEnvironment::getByUIDS(QStringList ids) const{
    QList< QSharedPointer<GWSEntity> > entities;
    foreach (QString id , ids) {
        entities.append( this->getByUID( id ) );
    }
    return entities;
}

QList< QSharedPointer<GWSEntity> > GWSEntityEnvironment::getByUIDS(QJsonArray json) const{
    QStringList ids;
    foreach(QJsonValue v , json){
        ids.append( v.toString() );
    }
    return this->getByUIDS( ids );
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getByClassAndUID( QString class_name , QString id) const{
    if( this->environment_entities ){
        QSharedPointer<GWSEntity> obj = this->environment_entities->getByClassAndUID<GWSEntity>( class_name , id );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->environment_entities->getByClassAndName<GWSEntity>( class_name , agent_name );
}

QList< QSharedPointer<GWSEntity> > GWSEntityEnvironment::getByClass( QString class_name ) const{
    return this->environment_entities->getByClass<GWSEntity>( class_name );
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getByName( QString agent_name ) const{
    return this->environment_entities->getByName<GWSEntity>( agent_name );
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSEntityEnvironment::registerEntity( QSharedPointer<GWSEntity> entity){
    GWSEnvironment::registerEntity( entity );

    if( entity->getProperty( SKIP_INDEXING ).toBool() ){
        return;
    }

    emit this->environment_entities->addObjectSignal( entity );
}

void GWSEntityEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity){
    GWSEnvironment::unregisterEntity( entity );
    emit this->environment_entities->removeObjectSignal( entity );
}

