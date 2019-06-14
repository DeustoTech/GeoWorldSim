#include "ObjectStorage.h"

#include <QJsonArray>

GWSObjectStorage::GWSObjectStorage() : QObject(){

}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSObjectStorage::isEmpty() const{
    QString minimum_level = QObject::staticMetaObject.className();
    if( this->objects.keys().contains( minimum_level.toStdString() ) ){
        return false;
    }
    return true;
}

quint64 GWSObjectStorage::getAmount() const{
    if( this != Q_NULLPTR && this->classes_stored.contains( QObject::staticMetaObject.className() ) ){
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

QSharedPointer<QObject> GWSObjectStorage::getByClassAndUID( const QString &class_name , const QString &uid ) const{
    if ( this->classes_stored.contains( class_name ) ){
        return this->object_uids.value( class_name.toStdString() )->value( uid.toStdString() , Q_NULLPTR );
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByUID( const QString &uid ){
    foreach( QString class_name , this->classes_stored ) {
        QSharedPointer<QObject> obj = this->object_uids.value( class_name.toStdString() )->value( uid.toStdString() , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByClassAndName( const QString &class_name , const QString &name ) const{
    if ( this->classes_stored.contains( class_name ) ){
        QSharedPointer<QObject> obj = this->object_names.value( class_name.toStdString() )->value( name.toStdString() , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QList< QSharedPointer<QObject> > GWSObjectStorage::getByClass( const QString &class_name ) const{
    QList< QSharedPointer<QObject> > list;
    if( this->classes_stored.contains( class_name ) ){
        this->mutex.lock();
        foreach (QSharedPointer<QObject> o, *this->objects.value( class_name.toStdString() ) ) {
            list.append( o );
        }
        this->mutex.unlock();
    }
    return list;
}

QSharedPointer<QObject> GWSObjectStorage::getByName(const QString &name ) const{
    foreach( QString class_name , this->classes_stored){
        QSharedPointer<QObject> obj = this->object_names.value( class_name.toStdString() )->value( name.toStdString() , Q_NULLPTR );
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
            this->objects.insert( c.toStdString() , list );

            QMap< std::string , QSharedPointer<QObject> >* map = new QMap< std::string , QSharedPointer<QObject> >();
            this->object_names.insert( c.toStdString() , map );

            QMap< std::string , QSharedPointer<QObject> >* map2 = new QMap< std::string , QSharedPointer<QObject> >();
            this->object_uids.insert( c.toStdString() , map2 );

            this->classes_stored.append( c );
        }
    }

    // Add to storage
    foreach( QJsonValue v , classes ){

        QString c = v.toString();
        if( c.isEmpty() ){ continue; }

        this->mutex.lock();

        this->objects[ c.toStdString() ]->append( object );
        this->object_names[ c.toStdString() ]->insert( object->objectName().toStdString() , object );
        this->object_uids[ c.toStdString() ]->insert( uid.toStdString() , object );

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

            this->objects.value( c.toStdString() )->removeAll( object );
            this->object_names.value( c.toStdString() )->remove( object->objectName().toStdString() );
            this->object_uids.value( c.toStdString() )->remove( uid.toStdString() );

            if( this->objects.value( c.toStdString() )->isEmpty() ){
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

