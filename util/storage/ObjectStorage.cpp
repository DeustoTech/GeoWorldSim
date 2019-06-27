#include "ObjectStorage.h"

#include <QJsonArray>

#include "../parallelism/ParallelismController.h"

GWSObjectStorage::GWSObjectStorage() : QObject(){
    this->connect( this , &GWSObjectStorage::addObjectSignal , this , &GWSObjectStorage::addObject );
    this->connect( this , &GWSObjectStorage::removeObjectSignal , this , &GWSObjectStorage::removeObject );
    this->connect( this , &GWSObjectStorage::deleteAllObjectsSignal , this , &GWSObjectStorage::deleteAllObjects );
}

GWSObjectStorage::~GWSObjectStorage(){
    foreach (std::string s , this->objects.keys() ) {
        delete this->objects.value( s );
    }
    foreach (std::string s , this->object_uids.keys() ) {
        delete this->object_uids.value( s );
    }
    foreach (std::string s , this->object_names.keys() ) {
        delete this->object_names.value( s );
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSObjectStorage::isEmpty() const{
    QString minimum_level = QObject::staticMetaObject.className();
    if( this->objects.value( minimum_level.toStdString() , Q_NULLPTR ) ){
        return false;
    }
    return true;
}

quint64 GWSObjectStorage::getAmount() const{
    if( this != Q_NULLPTR && this->objects.value( QObject::staticMetaObject.className() , Q_NULLPTR ) ){
        return this->objects.value( GWSObject::staticMetaObject.className() )->size();
    }
    return 0;
}

const QStringList GWSObjectStorage::getClasses() const{
    QStringList l;
    foreach (std::string s , this->objects.keys() ) {
        l.append( QString::fromStdString( s ) );
    }
    return l;
}

const QList< QSharedPointer<QObject> >* GWSObjectStorage::getAll() const{
    return this->objects.value( GWSObject::staticMetaObject.className() , Q_NULLPTR );
}

QSharedPointer<QObject> GWSObjectStorage::getByClassAndUID( const QString &class_name , const QString &uid ) const{
    if ( this->objects.value( class_name.toStdString() , Q_NULLPTR ) ){
        return this->object_uids.value( class_name.toStdString() )->value( uid.toStdString() , Q_NULLPTR );
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByUID( const QString &uid ){
    foreach( std::string class_name , this->objects.keys() ) {
        QSharedPointer<QObject> obj = this->object_uids.value( class_name )->value( uid.toStdString() , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

QSharedPointer<QObject> GWSObjectStorage::getByClassAndName( const QString &class_name , const QString &name ) const{
    if ( this->objects.value( class_name.toStdString() , Q_NULLPTR ) ){
        QSharedPointer<QObject> obj = this->object_names.value( class_name.toStdString() )->value( name.toStdString() , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    return Q_NULLPTR;
}

const QList< QSharedPointer<QObject> >* GWSObjectStorage::getByClass( const QString &class_name ) const{
    return this->objects.value( class_name.toStdString() );
}

const QList< QSharedPointer<QObject> >* GWSObjectStorage::getByClass(const QMetaObject &metaobject) const{
    return this->getByClass( metaobject.className() );
}

QSharedPointer<QObject> GWSObjectStorage::getByName(const QString &name ) const{
    this->mutex.lockForRead();
    foreach( std::string class_name , this->objects.keys() ){
        this->mutex.unlock();
        QSharedPointer<QObject> obj = this->object_names.value( class_name )->value( name.toStdString() , Q_NULLPTR );
        if( !obj.isNull() ){
            return obj;
        }
    }
    this->mutex.unlock();
    return Q_NULLPTR;
}

bool GWSObjectStorage::contains( QString class_name ) const{
    return this->objects.value( class_name.toStdString() , Q_NULLPTR ) != Q_NULLPTR;
}

bool GWSObjectStorage::contains( QSharedPointer<QObject> object ) const{
    QString c = object->metaObject()->className();
    if( !this->objects.value( c.toStdString() , Q_NULLPTR ) ){
        return false;
    }
    return this->objects.value( c.toStdString() )->contains( object );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSObjectStorage::addObject( QSharedPointer<QObject> object ){

    // Prepare for QObject which have no UIDs
    QJsonArray classes = QJsonArray({ QObject::staticMetaObject.className() });
    QString uid = QString("%1:%2").arg( object->metaObject()->className() ).arg( object->objectName() );

    // If GWSObject, we have UID
    QSharedPointer<GWSObject> gws_object = object.dynamicCast<GWSObject>();
    if( gws_object ){
        classes = gws_object->getInheritanceFamily();
        uid = gws_object->getUID();
    }

    // Add to storages
    foreach( QJsonValue v , classes ){

            std::string c = v.toString().toStdString();
            if( c.empty() ){ return; }

            this->mutex.lockForRead();
            QList< QSharedPointer<QObject> >* l = this->objects.value( c , Q_NULLPTR );

            if( !l ){
                this->mutex.unlock();

                this->mutex.lockForWrite();
                l = new QList< QSharedPointer<QObject> >();
                this->objects.insert( c , l );

                QMap< std::string , QSharedPointer<QObject> >* map = new QMap< std::string , QSharedPointer<QObject> >();
                this->object_names.insert( c , map );

                QMap< std::string , QSharedPointer<QObject> >* map2 = new QMap< std::string , QSharedPointer<QObject> >();
                this->object_uids.insert( c , map2 );
            }
            this->mutex.unlock();

            // Add to storage
            this->mutex.lockForWrite();

            l->append( object );
            this->object_names[ c ]->insert( object->objectName().toStdString() , object );
            this->object_uids[ c ]->insert( uid.toStdString() , object );

            this->mutex.unlock();
    }

}

void GWSObjectStorage::removeObject( QSharedPointer<QObject> object ){

    // Prepare for QObject which have no UIDs
    QJsonArray classes = QJsonArray({ QObject::staticMetaObject.className() });
    QString uid = QString("%1:%2").arg( object->metaObject()->className() ).arg( object->objectName() );

    // If GWSObject, we have UID
    QSharedPointer<GWSObject> gws_object = object.dynamicCast<GWSObject>();
    if( gws_object ){
        classes = gws_object->getInheritanceFamily();
        uid = gws_object->getUID();
    }

    // Remove from storages
    foreach( QJsonValue v , classes ){

            std::string c = v.toString().toStdString();
            if( c.empty() ){ return; }

            this->mutex.lockForRead();
            QList< QSharedPointer<QObject> >* l = this->objects.value( c , Q_NULLPTR );

            if( l ){
                this->mutex.unlock();

                this->mutex.lockForWrite();

                l->removeAll( object );
                this->object_names.value( c )->remove( object->objectName().toStdString() );
                this->object_uids.value( c )->remove( uid.toStdString() );
            }
            this->mutex.unlock();
    }
}

void GWSObjectStorage::deleteAllObjects(){

    QList< QSharedPointer<QObject> > * l = this->objects.value( QObject::metaObject()->className() , Q_NULLPTR );
    if( !l ){ return; }

    for( int i = 0 ; i < l->size() ; i ++ ){
        this->removeObject( l->at( i ) );
    }
}

