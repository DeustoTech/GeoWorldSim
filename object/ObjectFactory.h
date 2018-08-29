#ifndef GWSOBJECTFACTORY_H
#define GWSOBJECTFACTORY_H

#include <QMetaObject>
#include <QMap>
#include <QJsonObject>
#include <QSharedPointer>

#include "./../object/Object.h"

class GWSObjectFactory : public QObject
{
    Q_OBJECT

public:
    static GWSObjectFactory* globalInstance();

    // METHODS
    void registerType( const QMetaObject metaobject );
    const QMetaObject getRegisteredType( QString type_name );
    QSharedPointer<GWSObject> fromType( QString type , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );
    QSharedPointer<GWSObject> fromBytes( QByteArray bytes , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );
    QSharedPointer<GWSObject> fromJSON( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

protected:
    GWSObjectFactory();
    GWSObjectFactory(GWSObjectFactory const&);
    ~GWSObjectFactory();

private:
    QMap<QString , QMetaObject> constructors;
};

#endif // GWSOBJECTFACTORY_H
