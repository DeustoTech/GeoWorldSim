#include "ObjectFactory.h"

#include <QDebug>

#include "util/parallelism/ParallelismController.h"

GWSObjectFactory* GWSObjectFactory::globalInstance(){
    static GWSObjectFactory instance;
    return &instance;
}

GWSObjectFactory::GWSObjectFactory() : QObject( Q_NULLPTR ){
    this->moveToThread( GWSParallelismController::globalInstance()->getThread( qrand() ) );
}

GWSObjectFactory::~GWSObjectFactory(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSObjectFactory::registerObjectType( QMetaObject metaobject ){
    this->constructors.insert( metaobject.className() , metaobject );
}

const QMetaObject GWSObjectFactory::getRegisteredType( QString type_name ){
    return this->constructors.value( type_name );
}

GWSObject* GWSObjectFactory::createObject( QString type , GWSObject* parent ){
    QJsonObject json;
    json.insert( GWSObject::GWS_TYPE_PROP , type );
    return this->createObject( json , parent );
}

GWSObject* GWSObjectFactory::createObject( QJsonObject json , GWSObject* parent ){

    QString type = json.value( GWSObject::GWS_TYPE_PROP ).toString();

    if( !this->constructors.keys().contains( type ) ){
        qWarning() << QString("Object type %1 not defined in the ObjectFactory. To register use registerObjectType.").arg( type );
        return 0;
    }

    // Create object
    GWSObject* obj = dynamic_cast<GWSObject*>( this->constructors.value( type ).newInstance() );
    if( !obj ){ return 0; }
    obj->setParent( parent );

    // Call deserialize for further population
    obj->deserialize( json );

    // Return object
    return obj;
}
