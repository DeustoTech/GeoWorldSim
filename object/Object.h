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
    bool hasProperty( const QString &name ) const;
    QJsonValue operator[]( const QString &name ) const;

    // SETTERS
    //void setParent( QSharedPointer<GWSObject> parent );
    bool setProperty( const QString &name, const GWSUnit &value);
    bool setProperty( const QString &name, const QJsonValue &value);
    bool removeProperty( const QString &name );
    bool incrementProperty( QString &name , const QJsonValue &value);
    void copyProperties( GWSObject &other);

    // EVENTS
    // OLD METAPROPERTY SYSTEM bool event(QEvent *event);

    // PUBLIC DELETED ATTRIBUTE
    bool deleted = true; // IMPORTANT

signals:
    void entityPropertyChangedSignal( QString name );

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

};

Q_DECLARE_METATYPE( QJsonValue ) // TO STORE METAPROPERTIES
Q_DECLARE_METATYPE( QSharedPointer<GWSObject> ) // REQUIRED IN EVERY CHILD

#endif // GWSOBJECT_H
