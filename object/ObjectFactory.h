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
    const QMetaObject getRegisteredType( const QString &type_name );
    QSharedPointer<GWSObject> fromType( const QString &type , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );
    QSharedPointer<GWSObject> fromBytes( const QByteArray &bytes , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );
    QSharedPointer<GWSObject> fromJSON( const QJsonObject &json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // STATICS
    static QJsonValue simpleOrParentPropertyName( QString &property_name , QSharedPointer<GWSObject> object ,  QSharedPointer<GWSObject> parent );
    static QJsonValue simpleOrParentPropertyValue( const QJsonValue &property_value , QSharedPointer<GWSObject> object ,  QSharedPointer<GWSObject> parent );
    static QJsonValue incrementValue( const QJsonValue &existing_value , const QJsonValue &increment);

protected:
    GWSObjectFactory();
    GWSObjectFactory(GWSObjectFactory const&);
    ~GWSObjectFactory();

private:
    QMap<QString , QMetaObject> constructors;
};

#endif // GWSOBJECTFACTORY_H
