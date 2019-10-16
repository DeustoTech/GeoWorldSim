#ifndef GWSOBJECTFACTORY_H
#define GWSOBJECTFACTORY_H

#include <QMetaObject>
#include <QMap>
#include <QJsonObject>
#include <QSharedPointer>

#include "./../object/Object.h"

namespace geoworldsim {

class ObjectFactory : public QObject
{
    Q_OBJECT

public:
    static ObjectFactory* globalInstance();

    // METHODS
    void registerType( const QMetaObject metaobject );
    const QMetaObject getRegisteredType( const QString &type_name );
    QSharedPointer<Object> fromType( const QString &type , QSharedPointer<Object> parent = QSharedPointer<Object>() );
    QSharedPointer<Object> fromBytes( const QByteArray &bytes , QSharedPointer<Object> parent = QSharedPointer<Object>() );
    QSharedPointer<Object> fromJSON( const QJsonObject &json , QSharedPointer<Object> parent = QSharedPointer<Object>() );

    // STATICS
    static QJsonValue simpleOrParentPropertyName(const QString &property_name , QSharedPointer<Object> object ,  QSharedPointer<Object> parent );
    static QJsonValue simpleOrParentPropertyValue( const QJsonValue &property_value , QSharedPointer<Object> object ,  QSharedPointer<Object> parent );
    static QJsonValue incrementValue( const QJsonValue &existing_value , const QJsonValue &increment);

protected:
    ObjectFactory();
    ObjectFactory(ObjectFactory const&);
    ~ObjectFactory();

private:
    QMap<QString , QMetaObject> constructors;
};

}

#endif // GWSOBJECTFACTORY_H
