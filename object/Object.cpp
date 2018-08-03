#include "Object.h"

#include <QVector>
#include <QDebug>

#include "../../app/App.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../object/ObjectFactory.h"

QString GWSObject::GWS_ID_PROP = "@id";
QString GWSObject::GWS_TYPE_PROP = "@type";
QString GWSObject::GWS_INHERITANCE_FAMILY_PROP = "@family";
QString GWSObject::GWS_PARENT_PROP = "parent";

unsigned int GWSObject::counter = 0;

GWSObject::GWSObject( QObject *parent ) : QObject( parent ) , deleted(false) {
    QString generated_id = QString("%1-%2").arg( GWSApp::globalInstance()->getAppId() ).arg( ++GWSObject::counter );
    this->setProperty( GWS_ID_PROP ,  generated_id );
    this->setObjectName( generated_id );
}

GWSObject::GWSObject(const GWSObject &other) : QObject( other.parent() ){
}

GWSObject::~GWSObject(){
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

        const char* property_name = this->dynamicPropertyNames().at( i );
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
            default:

                // case GWSOBJECT
                if( property_value.typeName() == "GWSObject*" ){
                    GWSObject* obj = qvariant_cast<GWSObject*>( property_value );
                    json.insert( property_name , obj->serializeMini() );
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

void GWSObject::deserialize(QJsonObject json){

    if( json.keys().contains( GWS_ID_PROP ) ){ this->setProperty( GWS_ID_PROP , json.value( GWS_ID_PROP ).toString() ); }
    if( json.keys().contains( GWS_ID_PROP ) ){ this->setProperty( GWS_TYPE_PROP , json.value( GWS_TYPE_PROP ).toString() ); }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) ){ this->setProperty( GWS_INHERITANCE_FAMILY_PROP , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray().toVariantList() ); }

    // Set properties
    foreach( QString property_name , json.keys() ){

        if( property_name.contains('@') ){ continue; }

        QJsonValue property_value = json.value( property_name );

        switch ( property_value.type() ) {
        case QJsonValue::String: {
                this->setProperty( property_name , property_value.toString() ); break; }
        case QJsonValue::Double: {
                this->setProperty( property_name , property_value.toDouble() ); break; }
        case QJsonValue::Bool: {
                this->setProperty( property_name , property_value.toBool() ); break; }
        case QJsonValue::Array: {
                //this->setProperty( property_name , property_value.toArray() );
                break; }
        case QJsonValue::Object: {

                QJsonObject json_object = property_value.toObject();
                GWSObject* obj = Q_NULLPTR;

                // If it makes reference to an existing agent
                if( json_object.keys().contains( GWS_TYPE_PROP ) && json_object.keys().contains( GWS_ID_PROP ) ){
                    obj = GWSAgentEnvironment::globalInstance()->getByClassAndId( json_object.value( GWS_TYPE_PROP ).toString() , json_object.value( GWS_ID_PROP ).toString() );
                }

                if( !obj && json_object.keys().contains( GWS_TYPE_PROP ) ){
                    obj = GWSObjectFactory::globalInstance()->fromJSON( property_value.toObject() , this );
                }

                if( !obj ){ break; }

                QVariant obj_variant = QVariant::fromValue<GWSObject*>( obj );
                this->setProperty( property_name , obj_variant ); break; }
        default: {

                qDebug() << QString("Trying to deserialize Property (%1) of unknown type %2").arg( property_name ).arg( property_value.type() );
                this->setProperty( property_name , property_value.toVariant() ); break; }
        }
    }
}


/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSObject::getId() const{
    return this->getProperty( GWSObject::GWS_ID_PROP ).toString();
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

bool GWSObject::setProperty(const QString name, GWSObject *value){
    QVariant variant = QVariant::fromValue<GWSObject*>( value );
    return this->setProperty( name , variant );
}

void GWSObject::copyProperties(const GWSObject &other){
    for (int i = 0; i < other.dynamicPropertyNames().size(); ++i) {
        const char* property_name = other.dynamicPropertyNames().at( i );
        const QVariant property_value = other.getProperty( property_name );
        this->setProperty( property_name , property_value );
    }
}
