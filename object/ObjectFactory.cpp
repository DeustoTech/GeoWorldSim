#include "ObjectFactory.h"

#include <QDebug>

#include "../../agent/Agent.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"

#include "../../util/parallelism/ParallelismController.h"

GWSObjectFactory* GWSObjectFactory::globalInstance(){
    static GWSObjectFactory instance;
    return &instance;
}

GWSObjectFactory::GWSObjectFactory() : QObject( Q_NULLPTR ){
    this->moveToThread( GWSParallelismController::globalInstance()->getThread( qrand() ) );

    // Register basic types
    this->registerType( GWSAgent::staticMetaObject );
    this->registerType( GWSSkill::staticMetaObject );
    this->registerType( GWSBehaviour::staticMetaObject );
}

GWSObjectFactory::~GWSObjectFactory(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSObjectFactory::registerType( QMetaObject metaobject ){
    this->constructors.insert( metaobject.className() , metaobject );
}

const QMetaObject GWSObjectFactory::getRegisteredType( QString type_name ){
    return this->constructors.value( type_name );
}

GWSObject* GWSObjectFactory::create( QString type , GWSObject* parent ){
    QJsonObject json;
    json.insert( GWSObject::GWS_TYPE_PROP , type );
    return this->create( json , parent );
}

GWSObject* GWSObjectFactory::create( QJsonObject json , GWSObject* parent ){

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
