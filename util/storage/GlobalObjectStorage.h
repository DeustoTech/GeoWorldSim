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
    QSharedPointer<GWSObject> getByClassAndId( QMetaObject metaobject , QString internal_id) const;
    template <class T> QSharedPointer<T> getByClassAndId( QMetaObject metaobject , QString internal_id ) const;

    QSharedPointer<GWSObject>  getByClassAndName( QMetaObject metaobject , QString name ) const;
    template <class T> QSharedPointer<T> getByClassAndName( QMetaObject metaobject , QString name ) const;

    QList< QSharedPointer<GWSObject>  > getByClass( QMetaObject metaobject ) const;
    template <class T> QList<QSharedPointer<T>> getByClass( QMetaObject metaobject ) const;

    QSharedPointer<GWSObject>  getByName( QString name ) const;
    template <class T> QSharedPointer<T> getByName( QString name ) const {
        return this->storage->getByName<T>( name );
    }

    // SETTERS
    void add( QSharedPointer<GWSObject> object );

private:

    GWSGlobalObjectStorage();
    GWSGlobalObjectStorage(GWSGlobalObjectStorage const&);
    ~GWSGlobalObjectStorage();

    GWSObjectStorage* storage;

};

#endif // GWSGLOBALGSSOBJECTSTORAGE_H
