#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <QObject>
#include <QMetaProperty>
#include <QJsonValue>

class GWSObject : public QObject
{
    Q_OBJECT

    friend class GWSObjectFactory;
    friend class GWSEnvironment;
    friend class GWSExecutionEnvironment;

public:
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
    const QVariant property( QString name ) const;
    const QVariant operator[]( QString name ) const;

    // SETTERS
    void setProperty( QString name , const QJsonValue &value );

protected:

    // Abstract object, cant be created
    explicit GWSObject( GWSObject* parent = 0 );

private:

    // AUTOINCREMENTAL FOR IDS
    static unsigned int counter;

    bool deleted = false;
};

#endif // GWSOBJECT_H
