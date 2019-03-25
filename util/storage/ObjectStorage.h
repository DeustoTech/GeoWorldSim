#ifndef GWSOBJECTSTORAGE_H
#define GWSOBJECTSTORAGE_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QStringList>
#include <QSharedPointer>

#include "../../object/Object.h"

class GWSObjectStorage : public GWSObject
{
    Q_OBJECT

public:
    explicit GWSObjectStorage();

    // GETTERS
    bool isEmpty() const;
    quint64 getAmount() const;
    const QStringList getClasses() const;

    template <class T>
    QList< QSharedPointer<T> > getAll() const;
    QList< QSharedPointer<GWSObject> > getAll() const;

    template <class T>
    QSharedPointer<T> getByClassAndUID( QString class_name , QString uid ) const;
    QSharedPointer<GWSObject> getByClassAndUID( QString class_name , QString uid ) const;
    QSharedPointer<GWSObject> getByUID( QString uid );

    template <class T>
    QSharedPointer<T> getByClassAndName( QString class_name , QString name ) const;
    QSharedPointer<GWSObject> getByClassAndName( QString class_name , QString name ) const;

    template <class T> QList< QSharedPointer<T> > getByClassCasted( QString class_name ) const{
        QList< QSharedPointer<T> > objs;
        foreach( QSharedPointer<GWSObject> o , this->getByClass( class_name ) ){
            objs.append( o.dynamicCast<T>() );
        }
        return objs;
    }
    QList< QSharedPointer<GWSObject> > getByClass( QString class_name ) const;

    template <class T>
    QSharedPointer<T> getByName( QString name ) const;
    QSharedPointer<GWSObject> getByName( QString name ) const;

    bool contains( QString class_name ) const;
    bool contains( QSharedPointer<GWSObject> object ) const;

    // SETTERS
    virtual void add( QSharedPointer<GWSObject> object );
    virtual void remove( QSharedPointer<GWSObject> object );
    virtual void deleteAll();

protected:

    /**
     * STORAGE
    **/
    QStringList classes_stored;
    QHash<QString, QList< QSharedPointer<GWSObject> >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QHash<QString, QHash< QString , QSharedPointer<GWSObject> >* > object_uids;  // QMAP< QMAP< UID , OBJECT>>
    QHash<QString, QHash< QString , QSharedPointer<GWSObject> >* > object_names;  // QHash<ClassName, QHash<NAME, OBJECT>>

    mutable QMutex mutex;
};

#endif // GWSOBJECTSTORAGE_H
