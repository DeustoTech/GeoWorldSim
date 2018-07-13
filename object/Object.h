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
    Q_INVOKABLE explicit GWSObject( GWSObject* parent = 0 );
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
    const QVariant property( QString name ) const;
    const QVariant operator[]( QString name ) const;

    // SETTERS
    bool setProperty(const QString name, const QVariant &value);
    template <class T> bool setObjectProperty(const QString name, const GWSObject &value);
    void copyProperties(const GWSObject &other );

private:

    // AUTOINCREMENTAL FOR IDS
    static unsigned int counter;

    bool deleted = false;
};

Q_DECLARE_METATYPE(GWSObject)

#endif // GWSOBJECT_H
