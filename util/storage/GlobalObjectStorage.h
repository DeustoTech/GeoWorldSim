#ifndef GLOBALGSSOBJECTSTORAGE_H
#define GLOBALGSSOBJECTSTORAGE_H

#include "../../object/Object.h"
#include "../../util/storage/ObjectStorage.h"

namespace geoworldsim {
namespace storage {


class GlobalObjectStorage : public Object
{
    Q_OBJECT

public:

    static GlobalObjectStorage* globalInstance();

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

    GlobalObjectStorage();
    GlobalObjectStorage(GlobalObjectStorage const&);
    ~GlobalObjectStorage();

    ObjectStorage* storage;

};

}
}

#endif // GLOBALGSSOBJECTSTORAGE_H
