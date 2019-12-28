#include "GlobalObjectStorage.h"

geoworldsim::storage::GlobalObjectStorage* geoworldsim::storage::GlobalObjectStorage::globalInstance(){
    static geoworldsim::storage::GlobalObjectStorage instance;
    return &instance;
}

geoworldsim::storage::GlobalObjectStorage::GlobalObjectStorage() : Object(  ){
    this->storage = new ObjectStorage();
}

geoworldsim::storage::GlobalObjectStorage::~GlobalObjectStorage(){
    this->storage->deleteLater();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QSharedPointer<QObject> geoworldsim::storage::GlobalObjectStorage::getByClassAndUID( const QMetaObject& metaobject , const QString& uid ) const{
    return this->storage->getByClassAndUID( metaobject.className() , uid );
}
template <class T> QSharedPointer<T> geoworldsim::storage::GlobalObjectStorage::getByClassAndUID( const QMetaObject& metaobject , const QString& uid  ) const{
    return this->storage->getByClassAndUID<T>( metaobject , uid );
}

QSharedPointer<QObject> geoworldsim::storage::GlobalObjectStorage::getByClassAndName( const QMetaObject& metaobject , const QString& name ) const{
    return this->storage->getByClassAndName( metaobject.className() , name );
}
template <class T> QSharedPointer<T>geoworldsim::storage::GlobalObjectStorage::getByClassAndName(const QMetaObject &metaobject , const QString &name ) const{
    return this->storage->getByClassAndName<T>( metaobject , name );
}

const QList< QSharedPointer<QObject> >* geoworldsim::storage::GlobalObjectStorage::getByClass( const QMetaObject& metaobject ) const{
    return this->storage->getByClass( metaobject.className() );
}

template <class T> QList<QSharedPointer<T>> geoworldsim::storage::GlobalObjectStorage::getByClass( const QMetaObject& metaobject ) const{
    return this->storage->getByClass<T>( metaobject );
}

QSharedPointer<QObject> geoworldsim::storage::GlobalObjectStorage::getByName( const QString& name ) const{
    return this->storage->getByName( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::storage::GlobalObjectStorage::add( QSharedPointer<QObject> object ){
    this->storage->addObject( object );
}


