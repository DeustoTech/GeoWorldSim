#include "GlobalObjectStorage.h"

GWSGlobalObjectStorage* GWSGlobalObjectStorage::globalInstance(){
    static GWSGlobalObjectStorage instance;
    return &instance;
}

GWSGlobalObjectStorage::GWSGlobalObjectStorage() : GWSObject( Q_NULLPTR ){
    this->storage = new GWSObjectStorage();
}

GWSGlobalObjectStorage::~GWSGlobalObjectStorage(){
    this->storage->deleteLater();
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSObject* GWSGlobalObjectStorage::getByClassAndId( QMetaObject metaobject , QString id) const{
    return this->storage->getByClassAndId( metaobject , id );
}
template <class T> T* GWSGlobalObjectStorage::getByClassAndId( QMetaObject metaobject , QString id ) const{
    return this->storage->getByClassAndId<T>( metaobject , id );
}

GWSObject* GWSGlobalObjectStorage::getByClassAndName( QMetaObject metaobject , QString name ) const{
    return this->storage->getByClassAndName( metaobject , name );
}
template <class T> T* GWSGlobalObjectStorage::getByClassAndName( QMetaObject metaobject , QString name ) const{
    return this->storage->getByClassAndName<T>( metaobject , name );
}

QList< GWSObject* > GWSGlobalObjectStorage::getByClass( QMetaObject metaobject ) const{
    return this->storage->getByClass( metaobject );
}
template <class T> QList<T*> GWSGlobalObjectStorage::getByClass( QMetaObject metaobject ) const{
    return this->storage->getByClass<T>( metaobject );
}

GWSObject* GWSGlobalObjectStorage::getByName( QString name ) const{
    return this->storage->getByName( name );
}
template <class T> T* GWSGlobalObjectStorage::getByName( QString name ) const{
    return this->storage->getByName<T>( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGlobalObjectStorage::add(GWSObject *object ){
    this->storage->add( object );
}


