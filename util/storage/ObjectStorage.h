#ifndef OBJECTSTORAGE_H
#define OBJECTSTORAGE_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QMutex>
#include <QReadWriteLock>
#include <QStringList>
#include <QSharedPointer>

#include "../../object/Object.h"

namespace geoworldsim {
namespace storage {

class ObjectStorage : public QObject
{
    Q_OBJECT

public:
    explicit ObjectStorage();
    ~ObjectStorage();

    // GETTERS
    bool isEmpty() const;
    quint64 getAmount() const;
    const QStringList getClasses() const;

    template <class T>
    QList< QSharedPointer<T> > getMatches( const QString &class_name = "" , const QString &uid = "" ) const{
        QList< QSharedPointer<T> > objs;
        const QList< QSharedPointer<QObject> > l = this->getMatches( class_name , uid );
        for( int i = 0 ; i < l.size() ; i++ ){
            objs.append( l.at( i ).dynamicCast<T>() );
        }
        return objs;
    }

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
        if( !l ){ return objs; }
        for( int i = 0 ; i < l->size() ; i++ ){
            objs.append( l->at( i ).dynamicCast<T>() );
        }
        return objs;
    }

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
    QList< QSharedPointer<QObject> > getMatches( const QString &class_name = "" , const QString &uid = "" ) const; // FAST METHOD, AS IT ONLY RETURNS A POINTER
    const QList< QSharedPointer<QObject> >* getAll() const; // FAST METHOD, AS IT ONLY RETURNS A POINTER
    const QList< QSharedPointer<QObject> >* getByClass( const QString &class_name ) const; // FAST METHOD, AS IT ONLY RETURNS A POINTER
    const QList< QSharedPointer<QObject> >* getByClass( const QMetaObject& metaobject ) const; // FAST METHOD, AS IT ONLY RETURNS A POINTER


protected:

    /**
     * STORAGE
    **/
    QMap< std::string , QList< QSharedPointer<QObject> >* > objects;  // QMAP<ClassName, QList<OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_uids;  // QMAP< ClassName , QMAP< UID , OBJECT>>
    QMap< std::string , QMap< std::string , QSharedPointer<QObject> >* > object_names;  // QMAP<ClassName , QMAP< NAME, OBJECT>>
    int stored_amount = 0;

    mutable QReadWriteLock mutex;
};

}
}

#endif // OBJECTSTORAGE_H
