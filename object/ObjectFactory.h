#ifndef GWSOBJECTFACTORY_H
#define GWSOBJECTFACTORY_H

#include <QMetaObject>
#include <QMap>
#include <QJsonObject>

#include "object/Object.h"

class GWSObjectFactory : public QObject
{
    Q_OBJECT

public:
    static GWSObjectFactory* globalInstance();

    // METHODS
    void registerObjectType( const QMetaObject metaobject );
    const QMetaObject getRegisteredType( QString type_name );
    GWSObject* createObject( QString type , GWSObject* parent = Q_NULLPTR );
    GWSObject* createObject( QJsonObject json = QJsonObject() , GWSObject* parent = Q_NULLPTR );


private:
    GWSObjectFactory();
    GWSObjectFactory(GWSObjectFactory const&);
    ~GWSObjectFactory();

    QMap<QString , QMetaObject> constructors;
};

#endif // GWSOBJECTFACTORY_H
