#include "ObjectFactory.h"

#include <QDebug>

#include "../../agent/Agent.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"
#include "../../util/uistyle/Style.h"

#include "../../util/geometry/Point.h"
#include "../../util/geometry/LineString.h"
#include "../../util/geometry/Polygon.h"

#include "../../util/grid/Grid.h"

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

/*GWSObject* GWSObjectFactory::create( QString type , GWSObject* parent ){
    QJsonObject json;
    json.insert( GWSObject::GWS_TYPE_PROP , type );
    return this->create( json , parent );
}*/

GWSObject* GWSObjectFactory::fromType( QString type , GWSObject* parent ){
    QJsonObject json;
    json.insert( GWSObject::GWS_TYPE_PROP , type );
    return this->fromJSON( json , parent );
}

GWSObject* GWSObjectFactory::fromJSON( QJsonObject json , GWSObject* parent ){

    if( json.isEmpty() ){
        qDebug() << QString("Object Factory received empty JSON");
        return 0;
    }

    QString type = json.value( GWSObject::GWS_TYPE_PROP ).toString();

    if( !this->constructors.keys().contains( type ) ){
        qWarning() << QString("Object type %1 not registered in the ObjectFactory.").arg( type );
        return 0;
    }

    // Create object
    GWSObject* obj = dynamic_cast<GWSObject*>( this->constructors.value( type ).newInstance() );
    if( !obj ){
        return 0;
    }

    // Set parent if any
    obj->setParent( parent );

    // Call deserialize for further population
    obj->deserialize( json );

    // Return object
    return obj;
}
