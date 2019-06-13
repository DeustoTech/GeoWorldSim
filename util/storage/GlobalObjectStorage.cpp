#include "GlobalObjectStorage.h"

GWSGlobalObjectStorage* GWSGlobalObjectStorage::globalInstance(){
    static GWSGlobalObjectStorage instance;
    return &instance;
}

GWSGlobalObjectStorage::GWSGlobalObjectStorage() : GWSObject(  ){
    this->storage = new GWSObjectStorage();
}

GWSGlobalObjectStorage::~GWSGlobalObjectStorage(){
    this->storage->deleteLater();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<QObject> GWSGlobalObjectStorage::getByClassAndId( QMetaObject metaobject , QString id) const{
    return this->storage->getByClassAndUID( metaobject.className() , id );
}
template <class T> QSharedPointer<T> GWSGlobalObjectStorage::getByClassAndId( QMetaObject metaobject , QString id ) const{
    return this->storage->getByClassAndUID<T>( metaobject , id );
}

QSharedPointer<QObject> GWSGlobalObjectStorage::getByClassAndName( QMetaObject metaobject , QString name ) const{
    return this->storage->getByClassAndName( metaobject.className() , name );
}
template <class T> QSharedPointer<T>GWSGlobalObjectStorage::getByClassAndName( QMetaObject metaobject , QString name ) const{
    return this->storage->getByClassAndName<T>( metaobject , name );
}

QList< QSharedPointer<QObject> > GWSGlobalObjectStorage::getByClass( QMetaObject metaobject ) const{
    return this->storage->getByClass( metaobject.className() );
}
template <class T> QList<QSharedPointer<T>> GWSGlobalObjectStorage::getByClass( QMetaObject metaobject ) const{
    return this->storage->getByClass<T>( metaobject );
}

QSharedPointer<QObject> GWSGlobalObjectStorage::getByName( QString name ) const{
    return this->storage->getByName( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGlobalObjectStorage::add( QSharedPointer<QObject> object ){
    this->storage->add( object );
}


