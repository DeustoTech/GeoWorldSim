#include "ObjectFactory.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "../../app/App.h"
#include "../../agent/Entity.h"
#include "../../util/parallelism/ParallelismController.h"

GWSObjectFactory* GWSObjectFactory::globalInstance(){
    static GWSObjectFactory instance;
    return &instance;
}

GWSObjectFactory::GWSObjectFactory() : QObject( Q_NULLPTR ){

    // Register basic types
    this->registerType( GWSEntity::staticMetaObject );
    //this->registerType( GWSSkill::staticMetaObject );
    //this->registerType( GWSBehaviour::staticMetaObject );
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
    json.insert( GWSObject::GWS_CLASS_PROP , type );
    return this->fromJSON( json , parent );
}

QSharedPointer<GWSObject> GWSObjectFactory::fromBytes(QByteArray json_bytes, QSharedPointer<GWSObject> parent){
    QJsonObject object = QJsonDocument::fromJson( json_bytes ).object();
    return this->fromJSON( object , parent );
}

QSharedPointer<GWSObject> GWSObjectFactory::fromJSON( QJsonObject json , QSharedPointer<GWSObject> parent ){

    if( json.isEmpty() ){
        qWarning() << QString("Object Factory received empty JSON");
        GWSApp::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    if( !json.keys().contains( GWSObject::GWS_CLASS_PROP ) ){
        qWarning() << QString("Object Factory received json without %1").arg( GWSObject::GWS_CLASS_PROP );
        GWSApp::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    QString class_name = json.value( GWSObject::GWS_CLASS_PROP ).toString();

    if( !this->constructors.keys().contains( class_name ) ){
        qWarning() << QString("Object class %1 not registered in the ObjectFactory.").arg( class_name );
        GWSApp::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    // Create object
    GWSObject* obj_raw = dynamic_cast<GWSObject*>( this->constructors.value( class_name ).newInstance() );
    if( !obj_raw ){
        return Q_NULLPTR;
    }

    // CREATE QSHAREPOINTERS!! DO NOT DELETE THEM, CALL CLEAR() INSTEAD
    QSharedPointer<GWSObject> obj = QSharedPointer<GWSObject>( obj_raw );
    obj_raw->self_shared_pointer = obj;
    obj_raw->setProperty( GWSObject::GWS_UID_PROP , QString("%1::%2").arg( class_name ).arg( ++GWSObject::counter ) );

    // Set parent if any
    if( parent ){
        obj->setProperty( GWSObject::GWS_UID_PROP , QString("%1::%2::%3").arg( parent->getUID() ).arg( class_name ).arg( ++GWSObject::counter ) );
    }

    // Call deserialize for further population
    obj->deserialize( json , parent );

    // Call initalize hook
    obj->afterCreateHook();

    // Return object
    return obj;
}

QJsonValue GWSObjectFactory::incrementValue(QJsonValue existing_value, QJsonValue increment){

    if( existing_value.isNull() ){
        return increment;
    }

    if( increment.isNull() ){
        return existing_value;
    }

    // Unitary elements (int, double, string, bool)

    if( existing_value.isDouble() ){
        return existing_value.toDouble() + increment.toDouble();
    }

    if( existing_value.isString() ){
        return existing_value.toString() + increment.toString();
    }

    if( existing_value.isBool() ){
        return existing_value.toBool() + increment.toBool();
    }

    // Complex element ARRAY

    {
        if( existing_value.isArray() ){
            QJsonArray existing_array = existing_value.toArray();

            if( increment.isArray() ){
                foreach( QJsonValue v , increment.toArray() ){
                    existing_array.append( v );
                }
            } else {
                existing_array.append( increment );
            }
            return existing_array;
        }
    }

    // Complext elements OBJECT

    QJsonObject result;

    if( existing_value.isObject() ){
        foreach( QString key , existing_value.toObject().keys() ){
            result.insert( key , GWSObjectFactory::incrementValue( existing_value.toObject()[key] , increment.toObject()[ key ] ) );
        }
    }

    if( increment.isObject() ){
        foreach( QString key , increment.toObject().keys() ){
            result.insert( key , GWSObjectFactory::incrementValue( existing_value.toObject()[key] , increment.toObject()[ key ] ) );
        }
    }

    return result;
}
