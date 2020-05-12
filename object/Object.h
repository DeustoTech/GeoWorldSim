#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QMetaProperty>
#include <QJsonObject>
#include <QJsonValue>
#include "../../util/units/Units.h"

namespace geoworldsim {

namespace environment {
    class Environment;
}

class Object : public QObject
{
    Q_OBJECT // REQUIRED IN EVERY CHILD

    friend class ObjectFactory;
    friend class environment::Environment;

public:
    Q_INVOKABLE explicit Object();
    Object(const Object&);
    ~Object();

    // PROPERTIES
    static QString GWS_UID_PROP;
    static QString GWS_CLASS_PROP;
    static QString GWS_INHERITANCE_FAMILY_PROP;

    // EXPORTERS
    QJsonObject serializeMini() const;
    virtual QJsonObject serialize() const;

    // IMPORTERS
    virtual void deserialize( const QJsonObject& json , QSharedPointer<Object> parent = QSharedPointer<Object>() );

    // GETTERS
    QString getUID() const;
    //QSharedPointer<GWSObject> getParent() const;
    QSharedPointer<Object> getSharedPointer() const;
    QJsonArray getInheritanceFamily() const;
    virtual QJsonValue getProperty( const QString &name ) const;
    virtual QJsonObject getProperties( const QStringList& names ) const;
    bool hasProperty( const QString &name ) const;
    QJsonValue operator[]( const QString &name ) const;

    // SETTERS
    //void setParent( QSharedPointer<GWSObject> parent );
    bool setProperty( const QString &name, const unit::Unit &value );
    bool setProperty( const QString &name, const QJsonValue &value );
    bool setProperties( const QJsonObject& obj );
    bool removeProperty( const QString &name );
    bool removeProperties( const QStringList& names );
    bool incrementProperty( QString &name , const QJsonValue &value );
    void copyProperties( Object &other);
    void addSubscription( const QString &property_name , std::function<void( QSharedPointer<Object> , QString )> callback );

    // PUBLIC DELETED ATTRIBUTE
    bool deleted = true; // IMPORTANT

protected:

    // HOOKS
    virtual void afterCreateHook(){}
    virtual void beforeDestroyHook(){}

private:

    QSharedPointer<Object> self_shared_pointer;

    // MUTEX
    mutable QReadWriteLock mutex;

    // AUTOINCREMENTAL FOR IDS
    static quint64 counter;

    QJsonObject* properties;
    QMap< QString , QList< std::function<void( QSharedPointer<Object> , QString )> > > subscriptions;

};

}

Q_DECLARE_METATYPE( QJsonValue ) // TO STORE METAPROPERTIES
Q_DECLARE_METATYPE( QSharedPointer<geoworldsim::Object> ) // REQUIRED TO BE USED IN SIGNAL AND SLOTS
Q_DECLARE_METATYPE( QSharedPointer<QObject> ) // REQUIRED TO BE USED IN SIGNAL AND SLOTS

#endif // GWSOBJECT_H
