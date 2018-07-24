#include "Object.h"

#include <QVector>
#include <QDebug>

#include "../../app/App.h"

#include "../../object/ObjectFactory.h"

QString GWSObject::GWS_ID_PROP = "@id";
QString GWSObject::GWS_TYPE_PROP = "@type";
QString GWSObject::GWS_INHERITANCE_FAMILY_PROP = "@family";

unsigned int GWSObject::counter = 0;

GWSObject::GWSObject( QObject *parent ) : QObject( parent ){
    QString generated_id = QString("SIM%1-OBJ%2").arg( GWSApp::globalInstance()->getAppId() ).arg( ++GWSObject::counter );
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
            case QVariant::LongLong:
                json.insert( property_name , this->getProperty( property_name ).toLongLong() ); break;
            case QVariant::Bool:
                json.insert( property_name , this->getProperty( property_name ).toBool() ); break;
            case QVariant::String:
                json.insert( property_name , this->getProperty( property_name ).toString() ); break;
            default:

                if( GWSObject* obj = qvariant_cast<GWSObject*>( property_value ) ){
                    json.insert( property_name , obj->serialize() );
                } else {
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

    // Set properties
    foreach( QString property_name , json.keys() ){

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
                GWSObject* obj = GWSObjectFactory::globalInstance()->fromJSON( property_value.toObject() , this );
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
