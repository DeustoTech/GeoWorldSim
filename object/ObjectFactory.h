#ifndef GWSOBJECTFACTORY_H
#define GWSOBJECTFACTORY_H

#include <QMetaObject>
#include <QMap>
#include <QJsonObject>

#include "./../object/Object.h"

class GWSObjectFactory : public QObject
{
    Q_OBJECT

public:
    static GWSObjectFactory* globalInstance();

    // METHODS
    void registerType( const QMetaObject metaobject );
    const QMetaObject getRegisteredType( QString type_name );
    GWSObject* fromType( QString type , GWSObject* parent = Q_NULLPTR );
    GWSObject* fromBytes( QByteArray bytes , GWSObject* parent = Q_NULLPTR );
    GWSObject* fromJSON( QJsonObject json , GWSObject* parent = Q_NULLPTR );

protected:
    GWSObjectFactory();
    GWSObjectFactory(GWSObjectFactory const&);
    ~GWSObjectFactory();

private:
    QMap<QString , QMetaObject> constructors;
};

#endif // GWSOBJECTFACTORY_H
