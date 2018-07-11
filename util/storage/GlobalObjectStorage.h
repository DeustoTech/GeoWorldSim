#ifndef GWSGLOBALGSSOBJECTSTORAGE_H
#define GWSGLOBALGSSOBJECTSTORAGE_H

#include "object/Object.h"

#include "util/storage/ObjectStorage.h"

class GWSGlobalObjectStorage : public GWSObject
{
    Q_OBJECT

public:

    static GWSGlobalObjectStorage* globalInstance();

    // GETTERS
    GWSObject* getByClassAndId( QMetaObject metaobject , QString internal_id) const;
    template <class T> T* getByClassAndId( QMetaObject metaobject , QString internal_id ) const;

    GWSObject* getByClassAndName( QMetaObject metaobject , QString name ) const;
    template <class T> T* getByClassAndName( QMetaObject metaobject , QString name ) const;

    QList< GWSObject* > getByClass( QMetaObject metaobject ) const;
    template <class T> QList<T*> getByClass( QMetaObject metaobject ) const;

    GWSObject* getByName( QString name ) const;
    template <class T> T* getByName( QString name ) const;

    // SETTERS
    void add( GWSObject* object );

private:

    GWSGlobalObjectStorage();
    GWSGlobalObjectStorage(GWSGlobalObjectStorage const&);
    ~GWSGlobalObjectStorage();

    GWSObjectStorage* storage;

};

#endif // GWSGLOBALGSSOBJECTSTORAGE_H
