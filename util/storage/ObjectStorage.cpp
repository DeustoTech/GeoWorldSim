#include "ObjectStorage.h"

#include <QJsonArray>

GWSObjectStorage::GWSObjectStorage() : QObject(){

}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSObjectStorage::isEmpty() const{
    if( this->objects.keys().contains( QObject::staticMetaObject.className() ) ){
        return this->objects.value( QObject::staticMetaObject.className() )->isEmpty();
    }
    return true;
}

quint64 GWSObjectStorage::getAmount() const{
    if( this && this->classes_stored.contains( QObject::staticMetaObject.className() ) ){
        return this->objects.value( QObject::staticMetaObject.className() )->size();
    }
    return 0;
}

const QStringList GWSObjectStorage::getClasses() const{
    return this->classes_stored;
}

QList< QSharedPointer<QObject> > GWSObjectStorage::getAll() const{
    return this->getByClass( QObject::staticMetaObject.className() );
}

template <class T>
QList< QSharedPointer<T> > GWSObjectStorage::getAll() const{
    QList< QSharedPointer<T> > list;
    foreach( QSharedPointer<QObject> obj , this->getAll() ){
        list.append( obj.dynamicCast<T>() );
    }
    return list;
}

QSharedPointer<QObject> GWSObjectStorage::getByClassAndUID( QString class_name , QString uid ) const{
    if ( this->classes_stored.contains( class_name ) ){
        QSharedPointer<QObject> obj = this->object_uids.value( class_name )->value( uid , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByUID( QString uid ){
    foreach( QString class_name , this->classes_stored ) {
        QSharedPointer<QObject> obj = this->object_uids.value( class_name )->value( uid , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByClassAndName( QString class_name , QString name ) const{
    if ( this->classes_stored.contains( class_name ) ){
        QSharedPointer<QObject> obj = this->object_names.value( class_name )->value( name , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QList< QSharedPointer<QObject> > GWSObjectStorage::getByClass( QString class_name ) const{
    QList< QSharedPointer<QObject> > list;
    if( this->classes_stored.contains( class_name ) ){
        this->mutex.lock();
        foreach (QSharedPointer<QObject> o, *this->objects.value( class_name ) ) {
            list.append( o );
        }
        this->mutex.unlock();
    }
    return list;
}

QSharedPointer<QObject> GWSObjectStorage::getByName( QString name ) const{
    foreach( QString class_name , this->classes_stored){
        QSharedPointer<QObject> obj = this->object_names.value( class_name )->value( name , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

bool GWSObjectStorage::contains( QString class_name ) const{
    return this->classes_stored.contains( class_name );
}

bool GWSObjectStorage::contains( QSharedPointer<QObject> object ) const{
    if( !this->classes_stored.contains( object->metaObject()->className() ) ){
        return false;
    }
    return this->objects.value( object->metaObject()->className() )->contains( object );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSObjectStorage::add( QSharedPointer<QObject> object ){

    QJsonArray classes = QJsonArray({ QObject::staticMetaObject.className() });
    QString uid = QString("%1:%2").arg( object->metaObject()->className() ).arg( object->objectName() );

    // Create storages
    QSharedPointer<GWSObject> gws_object = object.dynamicCast<GWSObject>();
    if( gws_object ){
        classes = gws_object->getInheritanceFamily();
        uid = gws_object->getUID();
    }
    foreach( QJsonValue v , classes ){

        QString c = v.toString();
        if( c.isEmpty() ){ continue; }

        if( !this->classes_stored.contains( c ) ){

            QList< QSharedPointer<QObject> >* list = new QList< QSharedPointer<QObject> >();
            this->objects.insert( c , list );

            QHash<QString , QSharedPointer<QObject> >* map = new QHash<QString , QSharedPointer<QObject> >();
            this->object_names.insert( c , map );

            QHash<QString , QSharedPointer<QObject> >* map2 = new QHash<QString , QSharedPointer<QObject> >();
            this->object_uids.insert( c , map2 );

            this->classes_stored.append( c );
        }
    }

    // Add to storage
    foreach( QJsonValue v , classes ){

        QString c = v.toString();
        if( c.isEmpty() ){ continue; }

        this->mutex.lock();

        this->objects[ c ]->append( object );
        this->object_names[ c ]->insert( object->objectName() , object );
        this->object_uids[ c ]->insert( uid , object );

        this->mutex.unlock();
    }
}

void GWSObjectStorage::remove( QSharedPointer<QObject> object ){

    QJsonArray classes = QJsonArray({ QObject::staticMetaObject.className() });
    QString uid = QString("%1:%2").arg( object->metaObject()->className() ).arg( object->objectName() );

    // Remove from storages
    QSharedPointer<GWSObject> gws_object = object.dynamicCast<GWSObject>();
    if( gws_object ){
        classes = gws_object->getInheritanceFamily();
        uid = gws_object->getUID();
    }

    foreach( QJsonValue v , classes ){

        QString c = v.toString();
        if( c.isEmpty() ){ continue; }

        if( this->classes_stored.contains( c ) ){
            this->mutex.lock();

            this->objects.value( c )->removeAll( object );
            this->object_names.value( c )->remove( object->objectName() );
            this->object_uids.value( c )->remove( uid );

            if( this->objects.value( c )->isEmpty() ){
                this->classes_stored.removeAll( c );
            }
            this->mutex.unlock();
        }
    }
}

void GWSObjectStorage::deleteAll(){
    foreach( QSharedPointer<QObject> obj , this->getAll() ){
        obj.clear();
    }
}

