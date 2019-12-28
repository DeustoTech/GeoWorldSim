#include "ObjectFactory.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "../../app/App.h"
#include "../../util/parallelism/ParallelismController.h"
#include "../../entity/Entity.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/graph/Edge.h"


geoworldsim::ObjectFactory* geoworldsim::ObjectFactory::globalInstance(){
    static geoworldsim::ObjectFactory instance;
    return &instance;
}

geoworldsim::ObjectFactory::ObjectFactory() : QObject( Q_NULLPTR ){

    // Register basic types
    // qRegisterMetaType is necessary to be used in SIGNAL/SLOTS
    this->registerType( QObject::staticMetaObject );
    qRegisterMetaType< QSharedPointer<QObject> >( "QSharedPointer<QObject>" );

    this->registerType( Object::staticMetaObject );
    qRegisterMetaType< Object >( Object::staticMetaObject.className() );
    qRegisterMetaType< QSharedPointer<Object> >( "QSharedPointer<Object>" );

    this->registerType( Entity::staticMetaObject );
    qRegisterMetaType< Entity >( Entity::staticMetaObject.className() );
    qRegisterMetaType< QSharedPointer< Entity > >( "QSharedPointer<Entity>" );

    this->registerType( skill::Skill::staticMetaObject );
    qRegisterMetaType< skill::Skill >( skill::Skill::staticMetaObject.className() );

    this->registerType( behaviour::Behaviour::staticMetaObject );
    qRegisterMetaType< behaviour::Behaviour >( behaviour::Behaviour::staticMetaObject.className() );

    qRegisterMetaType< geometry::Geometry >( "Geometry" );
    qRegisterMetaType< geometry::Coordinate >( "Coordinate" );
    qRegisterMetaType< graph::Edge >( "Edge" );
}

geoworldsim::ObjectFactory::~ObjectFactory(){
}

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::ObjectFactory::registerType( QMetaObject metaobject ){
    this->constructors.insert( QString( metaobject.className() ).split("::").last() , metaobject );
}

const QMetaObject geoworldsim::ObjectFactory::getRegisteredType( const QString &type_name ){
    return this->constructors.value( type_name );
}

QSharedPointer<geoworldsim::Object> geoworldsim::ObjectFactory::fromType( const QString &type , QSharedPointer<Object> parent ){
    QJsonObject json;
    json.insert( Object::GWS_CLASS_PROP , type );
    return this->fromJSON( json , parent );
}

QSharedPointer<geoworldsim::Object> geoworldsim::ObjectFactory::fromBytes( const QByteArray &json_bytes, QSharedPointer<Object> parent){
    QJsonObject object = QJsonDocument::fromJson( json_bytes ).object();
    return this->fromJSON( object , parent );
}

QSharedPointer<geoworldsim::Object> geoworldsim::ObjectFactory::fromJSON( const QJsonObject &json , QSharedPointer<Object> parent ){

    if( json.isEmpty() ){
        qWarning() << QString("Object Factory received empty JSON");
        App::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    if( !json.keys().contains( Object::GWS_CLASS_PROP ) ){
        qWarning() << QString("Object Factory received json without %1").arg( Object::GWS_CLASS_PROP );
        App::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    QString class_name = json.value( Object::GWS_CLASS_PROP ).toString();

    if( !this->constructors.keys().contains( class_name ) ){
        qWarning() << QString("Object class %1 not registered in the ObjectFactory.").arg( class_name );
        App::globalInstance()->exit( -1 );
        return Q_NULLPTR;
    }

    // Create object
    Object* obj_raw = dynamic_cast<Object*>( this->constructors.value( class_name ).newInstance() );
    if( !obj_raw ){
        return Q_NULLPTR;
    }

    // CREATE QSHAREPOINTERS!! DO NOT DELETE THEM, CALL CLEAR() INSTEAD
    QSharedPointer<Object> obj = QSharedPointer<Object>( obj_raw );
    obj_raw->self_shared_pointer = obj;
    obj_raw->setProperty( Object::GWS_UID_PROP , QString("%1::%2").arg( class_name ).arg( ++Object::counter ) );

    // Set parent if any
    if( parent ){
        obj->setProperty( Object::GWS_UID_PROP , QString("%1::%2::%3").arg( parent->getUID() ).arg( class_name ).arg( ++Object::counter ) );
        obj->moveToThread( parent->thread() );
    }

    // Call deserialize for further population
    obj->deserialize( json , parent );

    // Call initalize hook
    obj->afterCreateHook();

    // Return object
    return obj;
}

QJsonValue geoworldsim::ObjectFactory::simpleOrParentPropertyName( const QString &property_name, QSharedPointer<Object> object, QSharedPointer<Object> parent){

    // IS STRING AND <>

    // If it comes between '<>', it is not the property name, but a kew to fetch said property name from one entities's value
    if( property_name.startsWith("<") && property_name.endsWith(">") ){
        if( !parent ){ return QJsonValue::Null; }
        QString str = property_name;
        str = str.remove( 0 , 1 );
        str.remove( str.length() - 1 , 1 );
        return parent->getProperty( str );
    }

    // IS SIMPLE STRING
    if( !object ){ return QJsonValue::Null; }
    return geoworldsim::ObjectFactory::simpleOrParentPropertyValue( object->Object::getProperty( property_name ) , object , parent );

}

QJsonValue geoworldsim::ObjectFactory::simpleOrParentPropertyValue( const QJsonValue &property_value ,  QSharedPointer<Object> object , QSharedPointer<Object> parent){

    // IS NULL

    if( property_value.isNull() ){
        return property_value;
    }

    // IS STRING AND <>

    QString value_as_string = property_value.toString();

    // If it comes between '<>', it is not the property name, but a kew to fetch said property name from one entities's value
    if( value_as_string.startsWith("<") && value_as_string.endsWith(">") ){
        return geoworldsim::ObjectFactory::simpleOrParentPropertyName( value_as_string , object , parent );
    }

    // IS OBJECT

    if( property_value.isObject() ){

        QJsonObject obj;
        foreach( QString key , property_value.toObject().keys() ) {

            QString k = key;
            // If it comes between '<>', it is not the property name, but a kew to fetch said property name from one entities's value
            if( k.startsWith("<") && k.endsWith(">") ){
                k.remove( 0 , 1 );
                k.remove( k.length() - 1 , 1 );
                return parent->getProperty( k );
            }

            QJsonValue v = geoworldsim::ObjectFactory::simpleOrParentPropertyValue( property_value.toObject().value( key ) , object , parent );
            obj.insert( k , v );
        }
        return obj;
    }

    // IS ARRAY

    if( property_value.isArray() ){

        QJsonArray arr;
        foreach (QJsonValue v , property_value.toArray()) {
            arr.append( geoworldsim::ObjectFactory::simpleOrParentPropertyValue( v , object , parent ) );
        }

        return arr;
    }

    // ELSE

    return property_value;
}

QJsonValue geoworldsim::ObjectFactory::incrementValue( const QJsonValue &existing_value , const QJsonValue &increment){

    if( existing_value.isNull() ){
        return increment;
    }

    if( increment.isNull() ){
        return existing_value;
    }

    // Unitary elements (int, double, string, bool)

    if( existing_value.isDouble() ){
        return existing_value.toDouble( 0 ) + increment.toDouble( 0 );
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
            result.insert( key , geoworldsim::ObjectFactory::incrementValue( existing_value.toObject().value(key) , increment.toObject().value(key) ) );
        }
    }

    if( increment.isObject() ){
        foreach( QString key , increment.toObject().keys() ){
            result.insert( key , geoworldsim::ObjectFactory::incrementValue( existing_value.toObject()[key] , increment.toObject()[ key ] ) );
        }
    }

    return result;
}
