#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QMetaProperty>
#include <QJsonObject>
#include <QJsonValue>
#include "../../util/units/Units.h"

class GWSObject : public QObject
{
    Q_OBJECT // REQUIRED IN EVERY CHILD

    friend class GWSObjectFactory;
    friend class GWSEnvironment;
    friend class GWSExecutionEnvironment;

public:
    Q_INVOKABLE explicit GWSObject();
    GWSObject(const GWSObject&);
    ~GWSObject();

    // PROPERTIES
    static QString GWS_UID_PROP;
    static QString GWS_CLASS_PROP;
    static QString GWS_INHERITANCE_FAMILY_PROP;

    // EXPORTERS
    QJsonObject serializeMini() const;
    virtual QJsonObject serialize() const;

    // IMPORTERS
    virtual void deserialize( const QJsonObject& json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // GETTERS
    QString getUID() const;
    //QSharedPointer<GWSObject> getParent() const;
    QSharedPointer<GWSObject> getSharedPointer() const;
    QJsonArray getInheritanceFamily() const;
    virtual QJsonValue getProperty( const QString &name ) const;
    virtual QJsonObject getProperties( const QStringList& names ) const;
    bool hasProperty( const QString &name ) const;
    QJsonValue operator[]( const QString &name ) const;

    // SETTERS
    //void setParent( QSharedPointer<GWSObject> parent );
    bool setProperty( const QString &name, const GWSUnit &value );
    bool setProperty( const QString &name, const QJsonValue &value );
    bool setProperties( const QJsonObject& obj );
    bool removeProperty( const QString &name );
    bool removeProperties( const QStringList& names );
    bool incrementProperty( QString &name , const QJsonValue &value );
    void copyProperties( GWSObject &other);
    void addSubscription( const QString &property_name , std::function<void( QSharedPointer<GWSObject> , QString )> callback );

    // PUBLIC DELETED ATTRIBUTE
    bool deleted = true; // IMPORTANT

protected:

    // HOOKS
    virtual void afterCreateHook(){};
    virtual void beforeDestroyHook(){};

private:

    QSharedPointer<GWSObject> self_shared_pointer;

    // MUTEX
    mutable QReadWriteLock mutex;

    // AUTOINCREMENTAL FOR IDS
    static quint64 counter;

    QJsonObject* properties;
    QMap< QString , QList< std::function<void( QSharedPointer<GWSObject> , QString )> > > subscriptions;

};

Q_DECLARE_METATYPE( QJsonValue ) // TO STORE METAPROPERTIES
Q_DECLARE_METATYPE( QSharedPointer<GWSObject> ) // REQUIRED IN EVERY CHILD

#endif // GWSOBJECT_H
