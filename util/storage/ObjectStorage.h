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
    quint64 getAmount() const;
    const QStringList getClasses() const;

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
    unsigned int getCount();

    // SETTERS
    virtual void add( GWSObject* object );
    virtual void remove( GWSObject* object );

protected:

    /**
     * STORAGE
    **/
    QStringList classes_stored;
    QHash<QString, QList< GWSObject* > > objects;  // QMAP<ClassName, QList<OBJECT>>
    QHash<QString, QHash< QString , GWSObject* > > object_ids;  // QMAP< QMAP< ID , OBJECT>>
    QHash<QString, QHash< QString , GWSObject* > > object_names;  // QHash<ClassName, QHash<NAME, OBJECT>>
    quint64 amount_stored = 0;

    QMutex mutex;
};

#endif // GWSOBJECTSTORAGE_H
