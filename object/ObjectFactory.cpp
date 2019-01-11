#include "ObjectFactory.h"

#include <QDebug>
#include <QJsonDocument>

#include "../../app/App.h"
#include "../../agent/Agent.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"
#include "../../util/geometry/Geometry.h"

#include "../../util/geometry/Point.h"
#include "../../util/geometry/LineString.h"
#include "../../util/geometry/Polygon.h"

#include "../../util/grid/Grid.h"
#include "../../environment/network_environment/NetworkEdge.h"


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
    this->registerType( GWSGeometry::staticMetaObject );
    this->registerType( GWSNetworkEdge::staticMetaObject );
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

QSharedPointer<GWSObject> GWSObjectFactory::fromType( QString type , QSharedPointer<GWSObject> parent ){
    QJsonObject json;
    json.insert( GWSObject::GWS_TYPE_PROP , type );
    return this->fromJSON( json , parent );
}

QSharedPointer<GWSObject> GWSObjectFactory::fromBytes(QByteArray json_bytes, QSharedPointer<GWSObject> parent){
    QJsonObject object = QJsonDocument::fromJson( json_bytes ).object();
    return this->fromJSON( object , parent );
}

QSharedPointer<GWSObject> GWSObjectFactory::fromJSON( QJsonObject json , QSharedPointer<GWSObject> parent ){

    if( json.isEmpty() ){
        qDebug() << QString("Object Factory received empty JSON");
        return Q_NULLPTR;
    }

    QString type = json.value( GWSObject::GWS_TYPE_PROP ).toString();

    if( !this->constructors.keys().contains( type ) ){
        qWarning() << QString("Object type %1 not registered in the ObjectFactory.").arg( type );
        return Q_NULLPTR;
    }

    // Create object
    GWSObject* obj_raw = dynamic_cast<GWSObject*>( this->constructors.value( type ).newInstance() );
    if( !obj_raw ){
        return Q_NULLPTR;
    }

    // CREATE QSHAREPOINTERS!! DO NOT DELETE THEM, CALL CLEAR() INSTEAD
    QSharedPointer<GWSObject> obj = QSharedPointer<GWSObject>( obj_raw );
    obj_raw->self_shared_pointer = obj;
    obj_raw->setProperty( GWSObject::GWS_ID_PROP , QString("%1::%2::%3").arg( GWSApp::globalInstance()->getAppId() ).arg( type ).arg( ++GWSObject::counter ) );

    // Set parent if any
    if( parent ){
        obj->setProperty( GWSObject::GWS_ID_PROP , QString("%1::%2::%3").arg( parent->getId() ).arg( type ).arg( ++GWSObject::counter ) );
    }

    // Call deserialize for further population
    obj->deserialize( json , parent );

    // Call initalize hook
    obj->initialize();

    // Return object
    return obj;
}
