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
    return this->getRandomByClass<GWSEntity>( class_name );
}

template <class T> QSharedPointer<T> GWSEntityEnvironment::getRandomByClass( QString class_name ){
    QList< QSharedPointer<T> > all = this->getByClass<T>( class_name );
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
    return this->getByClassAndUID<GWSEntity>( class_name , id );
}

template <class T> QSharedPointer<T> GWSEntityEnvironment::getByClassAndUID( QString class_name , QString id ) const{
    if( this->environment_entities ){
        QSharedPointer<GWSObject> obj = this->environment_entities->getByClassAndUID( class_name , id );
        if( !obj.isNull() ){
            try {
                return obj.dynamicCast<T>();
            } catch(...){}

        }
    }
    return 0;
}

QSharedPointer<GWSEntity> GWSEntityEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->getByClassAndName<GWSEntity>( class_name , agent_name );
}

template <class T> QSharedPointer<T> GWSEntityEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->environment_entities->getByClassAndName( class_name , agent_name ).dynamicCast<T>();
}

QList< QSharedPointer<GWSEntity> > GWSEntityEnvironment::getByClass( QString class_name ) const{
    return this->getByClass<GWSEntity>( class_name );
}

template <class T> QList<QSharedPointer<T> > GWSEntityEnvironment::getByClass( QString class_name ) const{
    QList< QSharedPointer<T> > entities;
    foreach( QSharedPointer<GWSObject> o , this->environment_entities->getByClass( class_name ) ){
        entities.append( o.dynamicCast<T>() );
    }
    return entities;
}


QSharedPointer<GWSEntity> GWSEntityEnvironment::getByName( QString agent_name ) const{
    return this->getByName<GWSEntity>( agent_name );
}

template <class T> QSharedPointer<T> GWSEntityEnvironment::getByName(QString agent_name) const{
    return this->environment_entities->getByName( agent_name ).dynamicCast<T>();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSEntityEnvironment::registerEntity( QSharedPointer<GWSEntity> entity){
    GWSEnvironment::registerEntity( entity );

    if( entity->getProperty( SKIP_INDEXING ).toBool() ){
        return;
    }

    this->environment_entities->add( entity );
}

void GWSEntityEnvironment::unregisterEntity( QSharedPointer<GWSEntity> entity){
    GWSEnvironment::unregisterEntity( entity );
    this->environment_entities->remove( entity );
}

