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

QSharedPointer<QObject> GWSGlobalObjectStorage::getByClassAndUID( const QMetaObject& metaobject , const QString& uid ) const{
    return this->storage->getByClassAndUID( metaobject.className() , uid );
}
template <class T> QSharedPointer<T> GWSGlobalObjectStorage::getByClassAndUID( const QMetaObject& metaobject , const QString& uid  ) const{
    return this->storage->getByClassAndUID<T>( metaobject , uid );
}

QSharedPointer<QObject> GWSGlobalObjectStorage::getByClassAndName( const QMetaObject& metaobject , const QString& name ) const{
    return this->storage->getByClassAndName( metaobject.className() , name );
}
template <class T> QSharedPointer<T>GWSGlobalObjectStorage::getByClassAndName(const QMetaObject &metaobject , const QString &name ) const{
    return this->storage->getByClassAndName<T>( metaobject , name );
}

const QList< QSharedPointer<QObject> >* GWSGlobalObjectStorage::getByClass( const QMetaObject& metaobject ) const{
    return this->storage->getByClass( metaobject.className() );
}

template <class T> QList<QSharedPointer<T>> GWSGlobalObjectStorage::getByClass( const QMetaObject& metaobject ) const{
    return this->storage->getByClass<T>( metaobject );
}

QSharedPointer<QObject> GWSGlobalObjectStorage::getByName( const QString& name ) const{
    return this->storage->getByName( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGlobalObjectStorage::add( QSharedPointer<QObject> object ){
    this->storage->addObject( object );
}


