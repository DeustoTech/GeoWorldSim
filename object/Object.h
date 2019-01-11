#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <QObject>
#include <QMutex>
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
    static QString GWS_SIM_ID_PROP;
    static QString GWS_ID_PROP;
    static QString GWS_TYPE_PROP;
    static QString GWS_INHERITANCE_FAMILY_PROP;
    static QString GWS_PARENT_PROP;

    // EXPORTERS
    QJsonObject serializeMini() const;
    virtual QJsonObject serialize() const;

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // GETTERS
    QString getId() const;
    //QSharedPointer<GWSObject> getParent() const;
    QSharedPointer<GWSObject> getSharedPointer() const;
    QJsonArray getInheritanceFamily() const;
    virtual const QJsonValue getProperty( QString name ) const;
    const QJsonValue operator[]( QString name ) const;

    // SETTERS
    //void setParent( QSharedPointer<GWSObject> parent );
    bool setProperty(const QString name, const GWSUnit &value);
    bool setProperty(const QString name, const QJsonValue &value);
    void copyProperties(const GWSObject &other );

    bool deleted = true; // IMPORTANT

protected:

    // HOOKS
    virtual void initialize(){};

private:

    QSharedPointer<GWSObject> self_shared_pointer;

    // MUTEX
    mutable QMutex mutex;

    // AUTOINCREMENTAL FOR IDS
    static quint64 counter;

};

Q_DECLARE_METATYPE( QJsonValue ) // TO STORE METAPROPERTIES
Q_DECLARE_METATYPE( QSharedPointer<GWSObject> ) // REQUIRED IN EVERY CHILD

#endif // GWSOBJECT_H
