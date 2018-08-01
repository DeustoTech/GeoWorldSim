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

QList<GWSObject*> GWSObjectStorage::getAll() const{
    return this->getByClass( GWSObject::staticMetaObject.className() );
}

template <class T> QList<T*> GWSObjectStorage::getAll() const{
    QList<T*> list;
    foreach( GWSObject* obj , this->getAll() ){
        list.append( dynamic_cast<T*>( obj ) );
    }
    return list;
}

GWSObject* GWSObjectStorage::getByClassAndId( QString class_name , QString id) const{
    return this->getByClassAndId<GWSObject>( class_name , id );
}

template <class T> T* GWSObjectStorage::getByClassAndId( QString class_name , QString id ) const{
    if ( this->classes_stored.contains( class_name ) ){
         return dynamic_cast<T*>( this->object_ids[ class_name ].value( id , 0 ) );
    }
    return 0;
}

GWSObject* GWSObjectStorage::getByClassAndName( QString class_name , QString name ) const{
    return this->getByClassAndName<GWSObject>( class_name , name );
}

template <class T> T* GWSObjectStorage::getByClassAndName( QString class_name , QString name ) const{
    if ( this->classes_stored.contains( class_name ) ){
         return dynamic_cast<T*>( this->object_names[ class_name ].value( name , 0 ) );
    }
    return 0;
}

QList< GWSObject* > GWSObjectStorage::getByClass( QString class_name ) const{
    return this->objects.value( class_name );
}

template <class T> QList<T*> GWSObjectStorage::getByClass( QString class_name ) const{
    QList<T*> objs;
    foreach( GWSObject* o , this->getByClass( class_name ) ){
        objs.append( dynamic_cast<T*>( o ) );
    }
    return objs;
}

GWSObject* GWSObjectStorage::getByName( QString name ) const{
    foreach(QString class_name , this->classes_stored){
        return this->object_names[ class_name ].value( name , 0 );
    }
    return 0;
}

template <class T> T* GWSObjectStorage::getByName( QString name ) const{
    return dynamic_cast<T*>( this->getByName( name ) );
}

bool GWSObjectStorage::contains( QString class_name ) const{
    return this->classes_stored.contains( class_name );
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
    QStringList classes = object->getInheritanceFamily();
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
    QStringList classes = object->getInheritanceFamily();
    foreach( QString c , classes ){
        this->mutex.lock();

        this->objects[ c ].removeAll( object );
        this->object_names[ c ].remove( object->objectName() );
        this->object_ids[ c ].remove( object->getId() );

        this->mutex.unlock();
    }

    this->amount_stored--;
}

