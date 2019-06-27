#ifndef GWSGLOBALGSSOBJECTSTORAGE_H
#define GWSGLOBALGSSOBJECTSTORAGE_H

#include "../../object/Object.h"

#include "../../util/storage/ObjectStorage.h"

class GWSGlobalObjectStorage : public GWSObject
{
    Q_OBJECT

public:

    static GWSGlobalObjectStorage* globalInstance();

    // GETTERS
    QSharedPointer<QObject> getByClassAndUID( const QMetaObject& metaobject , const QString& uid ) const;
    template <class T> QSharedPointer<T> getByClassAndUID( const QMetaObject& metaobject , const QString& uid ) const;

    QSharedPointer<QObject> getByClassAndName( const QMetaObject& metaobject , const QString& name ) const;
    template <class T> QSharedPointer<T> getByClassAndName( const QMetaObject& metaobject , const QString& name ) const;

    const QList< QSharedPointer<QObject> >* getByClass( const QMetaObject& metaobject ) const;
    template <class T> QList<QSharedPointer<T>> getByClass( const QMetaObject& metaobject ) const;

    QSharedPointer<QObject> getByName( const QString& name ) const;
    template <class T> QSharedPointer<T> getByName( const QString& name ) const {
        return this->storage->getByName<T>( name );
    }

    // SETTERS
    void add( QSharedPointer<QObject> object );

private:

    GWSGlobalObjectStorage();
    GWSGlobalObjectStorage(GWSGlobalObjectStorage const&);
    ~GWSGlobalObjectStorage();

    GWSObjectStorage* storage;

};

#endif // GWSGLOBALGSSOBJECTSTORAGE_H
