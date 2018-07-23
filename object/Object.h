#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <QObject>
#include <QMetaProperty>
#include <QJsonValue>

class GWSObject : public QObject
{
    Q_OBJECT // REQUIRED IN EVERY CHILD

    friend class GWSObjectFactory;
    friend class GWSEnvironment;
    friend class GWSExecutionEnvironment;

public:
    Q_INVOKABLE explicit GWSObject( QObject* parent = Q_NULLPTR );
    GWSObject(const GWSObject&);
    ~GWSObject();

    // PROPERTIES
    static QString GWS_ID_PROP;
    static QString GWS_TYPE_PROP;
    static QString GWS_INHERITANCE_TREE_PROP;

    // EXPORTERS
    virtual QJsonObject serializeMini() const;
    virtual QJsonObject serialize() const;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // GETTERS
    QString getId() const;
    QStringList getInheritanceTree() const;
    const QVariant getProperty( QString name ) const;
    const QVariant operator[]( QString name ) const;

    // SETTERS
    bool setProperty(const QString name, const QVariant &value);
    bool setProperty(const QString name, GWSObject* value);
    void copyProperties(const GWSObject &other );

private:

    // AUTOINCREMENTAL FOR IDS
    static unsigned int counter;

    bool deleted = false;
};

Q_DECLARE_METATYPE(GWSObject*) // REQUIRED IN EVERY CHILD

#endif // GWSOBJECT_H
