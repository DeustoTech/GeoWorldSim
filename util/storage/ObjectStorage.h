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
    QSharedPointer<T> getByClassAndUID( const QString &class_name , const QString &uid ) const{
        return this->getByClassAndUID( class_name , uid ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByClassAndUID( const QString &class_name , const QString &uid ) const;
    QSharedPointer<QObject> getByUID( const QString &uid );

    template <class T>
    QSharedPointer<T> getByClassAndName( const QString &class_name , const QString &name ) const {
        return this->getByClassAndName( class_name , name ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByClassAndName( const QString &class_name , const QString &name ) const;

    template <class T> QList< QSharedPointer<T> > getByClass( const QString &class_name ) const{
        QList< QSharedPointer<T> > objs;
        foreach( QSharedPointer<QObject> o , this->getByClass( class_name ) ){
            objs.append( o.dynamicCast<T>() );
        }
        return objs;
    }
    QList< QSharedPointer<QObject> > getByClass( const QString &class_name ) const;

    template <class T>
    QSharedPointer<T> getByName( const QString &name ) const {
        return this->getByName( name ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByName( const QString &name ) const;

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
    QMap< std::string , QList< QSharedPointer<QObject> >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_uids;  // QMAP< QMAP< UID , OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_names;  // QHash<ClassName, QHash<NAME, OBJECT>>

    mutable QMutex mutex;
};

#endif // GWSOBJECTSTORAGE_H
