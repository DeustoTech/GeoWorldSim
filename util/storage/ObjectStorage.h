#ifndef GWSOBJECTSTORAGE_H
#define GWSOBJECTSTORAGE_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QStringList>

#include "../../object/Object.h"

class GWSObjectStorage : public GWSObject
{
    Q_OBJECT

public:
    explicit GWSObjectStorage( GWSObject* parent = Q_NULLPTR );

    // GETTERS
    bool isEmpty() const;
    quint64 getAmount() const;
    const QStringList getClasses() const;

    QList<GWSObject*> getAll() const;
    template <class T> QList<T*> getAll() const;

    GWSObject* getByClassAndId( QString class_name , QString id ) const;
    template <class T> T* getByClassAndId( QString class_name , QString id ) const;

    GWSObject* getByClassAndName( QString class_name , QString name ) const;
    template <class T> T* getByClassAndName( QString class_name , QString name ) const;

    QList< GWSObject* > getByClass( QString class_name ) const;
    template <class T> QList<T*> getByClass( QString class_name ) const;

    GWSObject* getByName( QString name ) const;
    template <class T> T* getByName( QString name ) const;

    bool contains( QString class_name ) const;
    bool contains( GWSObject* object ) const;

    // SETTERS
    virtual void add( GWSObject* object );
    virtual void remove( GWSObject* object );
    virtual void deleteAll();

protected:

    /**
     * STORAGE
    **/
    QStringList classes_stored;
    QHash<QString, QList< GWSObject* >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QHash<QString, QHash< QString , GWSObject* >* > object_ids;  // QMAP< QMAP< ID , OBJECT>>
    QHash<QString, QHash< QString , GWSObject* >* > object_names;  // QHash<ClassName, QHash<NAME, OBJECT>>

    QMutex mutex;
};

#endif // GWSOBJECTSTORAGE_H
