#ifndef GWSOBJECTSTORAGE_H
#define GWSOBJECTSTORAGE_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QStringList>
#include <QSharedPointer>

#include "../../object/Object.h"

class GWSObjectStorage : public QObject
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
    QList< QSharedPointer<QObject> > getAll() const;

    template <class T>
    QSharedPointer<T> getByClassAndUID( QString class_name , QString uid ) const{
        return this->getByClassAndUID( class_name , uid ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByClassAndUID( QString class_name , QString uid ) const;
    QSharedPointer<QObject> getByUID( QString uid );

    template <class T>
    QSharedPointer<T> getByClassAndName( QString class_name , QString name ) const {
        return this->getByClassAndName( class_name , name ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByClassAndName( QString class_name , QString name ) const;

    template <class T> QList< QSharedPointer<T> > getByClass( QString class_name ) const{
        QList< QSharedPointer<T> > objs;
        foreach( QSharedPointer<QObject> o , this->getByClass( class_name ) ){
            objs.append( o.dynamicCast<T>() );
        }
        return objs;
    }
    QList< QSharedPointer<QObject> > getByClass( QString class_name ) const;

    template <class T>
    QSharedPointer<T> getByName( QString name ) const {
        return this->getByName( name ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByName( QString name ) const;

    bool contains( QString class_name ) const;
    bool contains( QSharedPointer<QObject> object ) const;

    // SETTERS
    virtual void add( QSharedPointer<QObject> object );
    virtual void remove( QSharedPointer<QObject> object );
    virtual void deleteAll();

protected:

    /**
     * STORAGE
    **/
    QStringList classes_stored;
    QHash<QString, QList< QSharedPointer<QObject> >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QHash<QString, QHash< QString , QSharedPointer<QObject> >* > object_uids;  // QMAP< QMAP< UID , OBJECT>>
    QHash<QString, QHash< QString , QSharedPointer<QObject> >* > object_names;  // QHash<ClassName, QHash<NAME, OBJECT>>

    mutable QMutex mutex;
};

#endif // GWSOBJECTSTORAGE_H
