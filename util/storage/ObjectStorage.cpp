#include "ObjectStorage.h"

GWSObjectStorage::GWSObjectStorage( GWSObject* parent ) : GWSObject( parent ){

}

/**********************************************************************
 GETTERS
**********************************************************************/

quint64 GWSObjectStorage::getAmount() const{
    return this->amount_stored;
}

const QStringList GWSObjectStorage::getClasses() const{
    return this->classes_stored;
}

GWSObject* GWSObjectStorage::getByClassAndId( QMetaObject metaobject , QString id) const{
    return this->getByClassAndId<GWSObject>( metaobject , id );
}

template <class T> T* GWSObjectStorage::getByClassAndId( QMetaObject metaobject , QString id ) const{
    if ( this->classes_stored.contains( metaobject.className() ) ){
         return dynamic_cast<T*>( this->object_ids[ metaobject.className() ].value( id , 0 ) );
    }
    return 0;
}

GWSObject* GWSObjectStorage::getByClassAndName( QMetaObject metaobject , QString name) const{
    return this->getByClassAndName<GWSObject>( metaobject , name );
}

template <class T> T* GWSObjectStorage::getByClassAndName( QMetaObject metaobject, QString name) const{
    if ( this->classes_stored.contains( metaobject.className() ) ){
         return dynamic_cast<T*>( this->object_names[ metaobject.className() ].value( name , 0 ) );
    }
    return 0;
}

QList< GWSObject* > GWSObjectStorage::getByClass( QMetaObject metaobject ) const{
    return this->objects.value( metaobject.className() );
}

template <class T> QList<T*> GWSObjectStorage::getByClass( QMetaObject metaobject ) const{
    QList<T*> objs;
    foreach( GWSObject* o , this->getByClass( metaobject ) ){
        objs.append( dynamic_cast<T*>( o ) );
    }
    return objs;
}

GWSObject* GWSObjectStorage::getByName(QString name) const{
    foreach(QString class_name , this->classes_stored){
        return this->object_names[ class_name ].value( name , 0 );
    }
    return 0;
}

template <class T> T* GWSObjectStorage::getByName(QString name) const{
    return dynamic_cast<T*>( this->getByName( name ) );
}

bool GWSObjectStorage::contains( QMetaObject metaobject ) const{
    return this->classes_stored.contains( metaobject.className() );
}

bool GWSObjectStorage::contains( GWSObject* object ) const{
    if( !this->classes_stored.contains( object->metaObject()->className() ) ){
        return false;
    }
    return this->objects.value( object->metaObject()->className() ).contains( object );
}

unsigned int GWSObjectStorage::getCount(){
    return this->objects.value( QObject::staticMetaObject.className() ).size();
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSObjectStorage::add( GWSObject* object ){

    // Create storages
    QStringList classes = object->getInheritanceTree();
    foreach( QString c , classes ){
        if( !this->classes_stored.contains( c ) ){

            QList< GWSObject* > list;
            this->objects.insert( c , list );

            QHash<QString , GWSObject* > map;
            this->object_names.insert( c , map );

            QHash<QString , GWSObject* > map2;
            this->object_ids.insert( c , map2 );

            this->classes_stored.append( c );
        }
    }

    // Add to storage
    foreach( QString c , classes ){
        this->mutex.lock();

        this->objects[ c ].append( object );
        this->object_names[ c ].insert( object->objectName() , object );
        this->object_ids[ c ].insert( object->getId() , object );

        this->mutex.unlock();
    }

    this->amount_stored++;

}

void GWSObjectStorage::remove( GWSObject* object ){

    // Remove from storage
    QStringList classes = object->getInheritanceTree();
    foreach( QString c , classes ){
        this->mutex.lock();

        this->objects[ c ].removeAll( object );
        this->object_names[ c ].remove( object->objectName() );
        this->object_ids[ c ].remove( object->getId() );

        this->mutex.unlock();
    }

    this->amount_stored--;
}

