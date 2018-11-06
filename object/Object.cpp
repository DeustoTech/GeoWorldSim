#include "Object.h"

#include <QVector>
#include <QDebug>
#include <QDateTime>

#include "../../app/App.h"
#include "../../util/geometry/Geometry.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../object/ObjectFactory.h"

QString GWSObject::GWS_ID_PROP = "@id";
QString GWSObject::GWS_TYPE_PROP = "@type";
QString GWSObject::GWS_INHERITANCE_FAMILY_PROP = "@family";
QString GWSObject::GWS_PARENT_PROP = "parent";

quint64 GWSObject::counter = QDateTime::currentMSecsSinceEpoch();

GWSObject::GWSObject() : QObject() , deleted(false) {
    QString generated_id = QString("%1::GWSObject::%2").arg( GWSApp::globalInstance()->getAppId() ).arg( ++GWSObject::counter );
    this->setProperty( GWS_ID_PROP ,  generated_id );
}

GWSObject::GWSObject(const GWSObject &other) : QObject(){
}

GWSObject::~GWSObject(){
    //qDebug() << QString("Deleting %1 %2").arg( this->metaObject()->className() ).arg( this->getId() );
    this->deleted = true;
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

/**
 * @brief GSSObject::toMiniJSON Called when asked for many objects
 */
QJsonObject GWSObject::serializeMini() const{
    QJsonObject json;
    json.insert( GWS_ID_PROP , this->getId() );
    json.insert( GWS_TYPE_PROP , this->metaObject()->className() );
    return json;
}

/**
 * @brief GSSObject::toJSON Called when asked for more info about this object.
 * Contains all the additional information of the object.
 * Can be overwritten if want to add extra variables to child classes
 * @return
 */
QJsonObject GWSObject::serialize() const{
    QJsonObject json = this->serializeMini();
    for (int i = 0; i < this->dynamicPropertyNames().size(); ++i) {

        QString property_name = this->dynamicPropertyNames().at( i );
        const QVariant property_value = this->getProperty( property_name );

        switch ( property_value.type() ) {
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::Double:
                json.insert( property_name , this->getProperty( property_name ).toDouble() ); break;
            case QVariant::LongLong:
                json.insert( property_name , this->getProperty( property_name ).toLongLong() ); break;
            case QVariant::Bool:
                json.insert( property_name , this->getProperty( property_name ).toBool() ); break;
            case QVariant::String:
                json.insert( property_name , this->getProperty( property_name ).toString() ); break;
            case QVariant::StringList:
            case QVariant::List: {
                QJsonArray array;
                foreach(QString str , this->getProperty( property_name ).toStringList() ){
                    array.append( str );
                }
                json.insert( property_name , array ); break;
            }
            default:

                // case GWSGeometry
                if( QString( property_value.typeName() ) == "GWSGeometry*" ){
                    GWSGeometry* obj = qvariant_cast<GWSGeometry*>( property_value );
                    if( obj ){ json.insert( property_name , obj->serialize() ); }
                }

                // case GWSObject
                else if( QString( property_value.typeName() ) == "GWSObject*" ){
                    GWSObject* obj = qvariant_cast<GWSObject*>( property_value );
                    if( obj ){ json.insert( property_name , obj->serialize() ); }
                }

                else {
                    qDebug() << QString("Trying to serialize Property (%1) of unknown type %2").arg( property_name ).arg( property_value.typeName() );
                    json.insert( property_name , this->getProperty( property_name ).toJsonValue() ); break;
                }

        }

    }
    return json;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSObject::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    Q_UNUSED( parent );

    if( json.keys().contains( GWS_ID_PROP ) ){ this->setProperty( GWS_ID_PROP , json.value( GWS_ID_PROP ).toString() ); }
    if( json.keys().contains( GWS_TYPE_PROP ) ){ this->setProperty( GWS_TYPE_PROP , json.value( GWS_TYPE_PROP ).toString() ); }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) ){
        QStringList family;
        foreach(QJsonValue v , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray() ){
            family.append( v.toString() );
        }
        this->setProperty( GWS_INHERITANCE_FAMILY_PROP , family );
    }

    // Set properties
    foreach( QString property_name , json.keys() ){

        // Avoid @ starting keywords
        if( property_name.contains('@') ){ continue; }

        QJsonValue property_value = json.value( property_name );
        this->deserializeProperty( property_name , property_value );

    }
}

void GWSObject::deserializeProperty( QString property_name, QJsonValue property_value ){

    switch ( property_value.type() ) {
    case QJsonValue::String: {
            this->setProperty( property_name , property_value.toString() ); break; }
    case QJsonValue::Double: {
            this->setProperty( property_name , property_value.toDouble() ); break; }
    case QJsonValue::Bool: {
            this->setProperty( property_name , property_value.toBool() ); break; }
    case QJsonValue::Array: {
            QVariantList list;
            foreach (QJsonValue v , property_value.toArray() ) {
                list.append( v.toVariant() );
            }
            this->setProperty( property_name , list );
            break; }
    case QJsonValue::Object: {

            QJsonObject property_value_object = property_value.toObject();
            QString subelement_type = property_value_object.value( GWS_TYPE_PROP ).toString();

            // It refers to a GWSObject (has @type and @id)
            if( !subelement_type.isEmpty() ){

                QString subelement_id = property_value_object.value( GWS_ID_PROP ).toString();
                QSharedPointer<GWSObject> obj;

                if( !subelement_id.isEmpty() ){
                    obj = GWSAgentEnvironment::globalInstance()->getByClassAndId( subelement_type , subelement_id );
                }

                if( obj.isNull() ){
                    obj = GWSObjectFactory::globalInstance()->fromJSON( property_value_object );
                }

                // Set object as the property
                if( obj ){
                    QVariant obj_variant = QVariant::fromValue< QSharedPointer<GWSObject> >( obj );
                    this->setProperty( property_name , obj_variant );
                }

            } else {

                // Concat name with value keys
                foreach( QString subproperty_name , property_value_object.keys() ){

                    // Avoid @ starting keywords
                    if( subproperty_name.contains('@') ){ continue; }

                    QJsonValue property_value = property_value_object.value( subproperty_name );
                    this->deserializeProperty( property_name + ":" + subproperty_name , property_value );
                }
            }
            break;
    }
    default: {

            qDebug() << QString("Trying to deserialize Property (%1) of unknown type %2").arg( property_name ).arg( property_value.type() );
            this->setProperty( property_name , property_value.toVariant() ); break; }
    }

}


/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSObject::getId() const{
    return this->getProperty( GWSObject::GWS_ID_PROP ).toString();
}

QSharedPointer<GWSObject> GWSObject::getSharedPointer() const{
    return this->self_shared_pointer;
}

QStringList GWSObject::getInheritanceFamily() const{
    QStringList inheritance_family;
    inheritance_family.append( this->metaObject()->className() );
    const QMetaObject* mobj = this->metaObject();
    while( mobj ){
        if( !inheritance_family.contains( mobj->className() ) ){
            inheritance_family.append( mobj->className() );
        }
        mobj = mobj->superClass();
    }
    foreach(QString s , this->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toStringList() ){
        if( !inheritance_family.contains( s ) ){
            inheritance_family.append( s );
        }
    }
    return inheritance_family;
}

const QVariant GWSObject::getProperty( QString name ) const{
    return QObject::property( name.toLatin1() );
}

const QVariant GWSObject::operator []( QString name ) const{
    return this->getProperty( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

bool GWSObject::setProperty(const QString name, const GWSUnit &value){
    return this->setProperty( name , value.number() );
}

bool GWSObject::setProperty(const QString name, const QVariant &value){
    return QObject::setProperty( name.toLatin1() , value );
}

bool GWSObject::setProperty(const QString name, QSharedPointer<GWSObject> value){
    QVariant variant = QVariant::fromValue< QSharedPointer<GWSObject> >( value );
    return this->setProperty( name , variant );
}

void GWSObject::copyProperties(const GWSObject &other){
    for (int i = 0; i < other.dynamicPropertyNames().size(); ++i) {
        const char* property_name = other.dynamicPropertyNames().at( i );
        const QVariant property_value = other.getProperty( property_name );
        this->setProperty( property_name , property_value );
    }
}
