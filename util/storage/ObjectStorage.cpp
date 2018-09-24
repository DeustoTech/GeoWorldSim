#include "ObjectStorage.h"

GWSObjectStorage::GWSObjectStorage() : GWSObject(){

}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSObjectStorage::isEmpty() const{
    if( this->objects.keys().contains( GWSObject::staticMetaObject.className() ) ){
        return this->objects.value( GWSObject::staticMetaObject.className() )->isEmpty();
    }
    return true;
}

quint64 GWSObjectStorage::getAmount() const{
    if( this && this->classes_stored.contains( GWSObject::staticMetaObject.className() ) ){
        return this->objects.value( GWSObject::staticMetaObject.className() )->size();
    }
    return 0;
}

const QStringList GWSObjectStorage::getClasses() const{
    return this->classes_stored;
}

QList<QSharedPointer<GWSObject>> GWSObjectStorage::getAll() const{
    return this->getByClass( GWSObject::staticMetaObject.className() );
}

template <class T>
QList< QSharedPointer<T> > GWSObjectStorage::getAll() const{
    QList< QSharedPointer<T> > list;
    foreach( QSharedPointer<GWSObject> obj , this->getAll() ){
        list.append( obj.dynamicCast<T>() );
    }
    return list;
}

QSharedPointer<GWSObject> GWSObjectStorage::getByClassAndId( QString class_name , QString id ) const{
    return this->getByClassAndId<GWSObject>( class_name , id );
}

template <class T>
QSharedPointer<T> GWSObjectStorage::getByClassAndId( QString class_name , QString id ) const{
    if ( this->classes_stored.contains( class_name ) ){
        QSharedPointer<GWSObject> obj = this->object_ids.value( class_name )->value( id , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj.dynamicCast<T>();
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<GWSObject> GWSObjectStorage::getById( QString id ){
    return this->getById( id );
}


QSharedPointer<GWSObject> GWSObjectStorage::getByClassAndName( QString class_name , QString name ) const{
    return this->getByClassAndName<GWSObject>( class_name , name );
}

template <class T>
QSharedPointer<T> GWSObjectStorage::getByClassAndName( QString class_name , QString name ) const{
    if ( this->classes_stored.contains( class_name ) ){
         return this->object_names[ class_name ]->value( name , 0 ).dynamicCast<T>();
    }
    return 0;
}

QList< QSharedPointer<GWSObject> > GWSObjectStorage::getByClass( QString class_name ) const{
    QList< QSharedPointer<GWSObject> > list;
    if( this->classes_stored.contains( class_name ) ){
        foreach (QSharedPointer<GWSObject> o, *this->objects.value( class_name ) ) {
            list.append( o );
        }
    }
    return list;
}

template <class T>
QList< QSharedPointer<T> > GWSObjectStorage::getByClassCasted( QString class_name ) const{
    QList< QSharedPointer<T> > objs;
    foreach( QSharedPointer<GWSObject> o , this->getByClass( class_name ) ){
        objs.append( o.dynamicCast<T>() );
    }
    return objs;
}

QSharedPointer<GWSObject> GWSObjectStorage::getByName( QString name ) const{
    foreach(QString class_name , this->classes_stored){
        return this->object_names[ class_name ]->value( name , 0 );
    }
    return 0;
}

template <class T>
QSharedPointer<T> GWSObjectStorage::getByName( QString name ) const{
    return this->getByName( name ).dynamicCast<T>();
}


bool GWSObjectStorage::contains( QString class_name ) const{
    return this->classes_stored.contains( class_name );
}

bool GWSObjectStorage::contains( QSharedPointer<GWSObject> object ) const{
    if( !this->classes_stored.contains( object->metaObject()->className() ) ){
        return false;
    }
    return this->objects.value( object->metaObject()->className() )->contains( object );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSObjectStorage::add( QSharedPointer<GWSObject> object ){

    // Create storages
    QStringList classes = object->getInheritanceFamily();
    foreach( QString c , classes ){
        if( !this->classes_stored.contains( c ) ){

            QList< QSharedPointer<GWSObject> >* list = new QList< QSharedPointer<GWSObject> >();
            this->objects.insert( c , list );

            QHash<QString , QSharedPointer<GWSObject> >* map = new QHash<QString , QSharedPointer<GWSObject> >();
            this->object_names.insert( c , map );

            QHash<QString , QSharedPointer<GWSObject> >* map2 = new QHash<QString , QSharedPointer<GWSObject> >();
            this->object_ids.insert( c , map2 );

            this->classes_stored.append( c );
        }
    }

    // Add to storage
    foreach( QString c , classes ){
        this->mutex.lock();

        this->objects[ c ]->append( object );
        this->object_names[ c ]->insert( object->objectName() , object );
        this->object_ids[ c ]->insert( object->getId() , object );

        this->mutex.unlock();
    }
}

void GWSObjectStorage::remove( QSharedPointer<GWSObject> object ){

    // Remove from storage
    QStringList classes = object->getInheritanceFamily();
    foreach( QString c , classes ){
        if( this->classes_stored.contains( c ) ){
            this->mutex.lock();

            this->objects.value( c )->removeAll( object );
            this->object_names.value( c )->remove( object->objectName() );
            this->object_ids.value( c )->remove( object->getId() );

            if( this->objects.value( c )->isEmpty() ){
                this->classes_stored.removeAll( c );
            }
            this->mutex.unlock();
        }
    }
}

void GWSObjectStorage::deleteAll(){
    foreach( QSharedPointer<GWSObject> obj , this->getAll() ){
        obj.clear();
    }
}

