#ifndef GWSOBJECTSTORAGE_H
#define GWSOBJECTSTORAGE_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QReadWriteLock>
#include <QStringList>
#include <QSharedPointer>

#include "../../object/Object.h"

class GWSObjectStorage : public QObject
{
    Q_OBJECT

public:
    explicit GWSObjectStorage();
    ~GWSObjectStorage();

    // GETTERS
    bool isEmpty() const;
    quint64 getAmount() const;
    const QStringList getClasses() const;

    const QList< QSharedPointer<QObject> >* getAll() const; // FAST METHOD, AS IT ONLY RETURNS A POINTER

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

    template <class T>
    QList< QSharedPointer<T> > getByClass( const QString &class_name ) const{
        QList< QSharedPointer<T> > objs;
        const QList< QSharedPointer<QObject> >* l = this->getByClass( class_name );
        for( int i = 0 ; i < l->size() ; i++ ){
            objs.append( l->at( i ).dynamicCast<T>() );
        }
        return objs;
    }
    const QList< QSharedPointer<QObject> > *getByClass( const QString &class_name ) const; // FAST METHOD, AS IT ONLY RETURNS A POINTER
    const QList< QSharedPointer<QObject> >* getByClass( const QMetaObject& metaobject ) const; // FAST METHOD, AS IT ONLY RETURNS A POINTER

    template <class T>
    QSharedPointer<T> getByName( const QString &name ) const {
        return this->getByName( name ).dynamicCast<T>();
    }
    QSharedPointer<QObject> getByName( const QString &name ) const;

    bool contains( QString class_name ) const;
    bool contains( QSharedPointer<QObject> object ) const;

public slots:
    virtual void addObject( QSharedPointer<QObject> object );
    virtual void removeObject( QSharedPointer<QObject> object );
    virtual void deleteAllObjects();

signals:

    void addObjectSignal( QSharedPointer<QObject> object );
    void removeObjectSignal( QSharedPointer<QObject> object );
    void deleteAllObjectsSignal();

protected:

    /**
     * STORAGE
    **/
    QMap< std::string , QList< QSharedPointer<QObject> >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_uids;  // QMAP< ClassName , QMAP< UID , OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_names;  // QMAP<ClassName , QMAP<NAME, OBJECT>>
    int stored_amount = 0;

    mutable QReadWriteLock mutex;
};

#endif // GWSOBJECTSTORAGE_H
