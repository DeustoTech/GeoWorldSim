#include "Object.h"

#include <QDebug>
#include <QDateTime>
#include <QEvent>
#include <QJsonArray>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

QString GWSObject::GWS_UID_PROP = "id";
QString GWSObject::GWS_CLASS_PROP = "@gwsclass";
QString GWSObject::GWS_INHERITANCE_FAMILY_PROP = "@gwsgroups";

quint64 GWSObject::counter = QDateTime::currentMSecsSinceEpoch();

GWSObject::GWSObject() : QObject() , deleted(false) {
    QString generated_id = QString("Entity::%1").arg( ++GWSObject::counter );
    this->setProperty( GWSObject::GWS_UID_PROP ,  generated_id );
}

GWSObject::GWSObject(const GWSObject &other) : QObject(){
}

GWSObject::~GWSObject(){
    this->beforeDestroyHook();
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
    json.insert( GWS_UID_PROP , this->getUID() );
    json.insert( GWS_CLASS_PROP , this->metaObject()->className() );
    json.insert( GWS_INHERITANCE_FAMILY_PROP , this->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray() );
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
        if ( property_name.startsWith( "@" ) ){ continue; }
        json.insert( property_name , GWSObject::getProperty( property_name ) );
    }
    return json;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSObject::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    Q_UNUSED( parent );

    // Required properties
    QStringList parsed_properties;
    if( json.keys().contains( GWS_UID_PROP ) ){      parsed_properties.append( GWS_UID_PROP ); this->setProperty( GWS_UID_PROP , json.value( GWS_UID_PROP ).toString() ); }
    if( json.keys().contains( GWS_CLASS_PROP ) ){   parsed_properties.append( GWS_CLASS_PROP ); this->setProperty( GWS_CLASS_PROP , json.value( GWS_CLASS_PROP ).toString() ); }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) && json.value( GWS_INHERITANCE_FAMILY_PROP ).isString() ){
        parsed_properties.append( GWS_INHERITANCE_FAMILY_PROP);
        QJsonArray arr;
        arr.append( json.value( GWS_INHERITANCE_FAMILY_PROP ).toString() );
        this->setProperty( GWS_INHERITANCE_FAMILY_PROP , arr );
    }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) && json.value( GWS_INHERITANCE_FAMILY_PROP ).isArray() ){
        parsed_properties.append( GWS_INHERITANCE_FAMILY_PROP );
        this->setProperty( GWS_INHERITANCE_FAMILY_PROP , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray() );
    }

    // Remaining properties
    foreach( QString property_name , json.keys() ){

        if( parsed_properties.contains( property_name ) ){
            continue; // Do not overwrite just parsed properties
        }

        // Get the value to be inserted
        QJsonValue propert_value = json.value( property_name );

        // If is complex JSON (with value, metadata, etc)
        if( propert_value.isObject() && propert_value.toObject().keys().contains("value") ){
            propert_value = propert_value.toObject().value( "value" );
        }

        // Set Property
        this->setProperty( property_name , propert_value );
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSObject::getUID() const{
    return this->getProperty( GWSObject::GWS_UID_PROP ).toString();
}

QSharedPointer<GWSObject> GWSObject::getSharedPointer() const{
    return this->self_shared_pointer;
}

QJsonArray GWSObject::getInheritanceFamily() const{
    QJsonArray inheritance_family;
    inheritance_family.append( this->metaObject()->className() );
    const QMetaObject* mobj = this->metaObject();
    while( mobj && mobj->className() != QObject::metaObject()->className() ){
        if( !inheritance_family.contains( mobj->className() ) ){
            inheritance_family.append( mobj->className() );
        }
        mobj = mobj->superClass();
    }

    // GWSGROUPS ARRAY
    foreach(QJsonValue s , this->getProperty( GWSObject::GWS_INHERITANCE_FAMILY_PROP ).toArray() ){
        if( !inheritance_family.contains( s ) ){
            inheritance_family.append( s );
        }
    }
    return inheritance_family;
}

const QJsonValue GWSObject::getProperty( QString name ) const{

    // If it comes between '<>', it is not the property name, but a key where to get the property name from
    if( name.startsWith( "<" ) && name.endsWith( ">" ) ){
        QString property_name = name.remove( 0 , 1 );
        property_name = property_name.remove( property_name.length() - 1 , 1 );
        name = this->getProperty( property_name ).toString();
    }

    this->mutex.lock();
    const QVariant variant = QObject::property( name.toUtf8() );
    QJsonValue value = QJsonValue::fromVariant( variant );
    this->mutex.unlock();
    return value;
}

const QJsonValue GWSObject::operator []( QString name ) const{
    return this->getProperty( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

bool GWSObject::setProperty(const QString name, const GWSUnit &value){
    return this->setProperty( name , value.number() );
}

bool GWSObject::setProperty(const QString name, const QJsonValue &value){
    this->mutex.lock();
    bool ok = QObject::setProperty( name.toLatin1() , value.toVariant() );
    this->mutex.unlock();
    return ok;
}

void GWSObject::copyProperties(const GWSObject &other){
    /*for (int i = 0; i < other.dynamicPropertyNames().size(); ++i) {
        const char* property_name = other.dynamicPropertyNames().at( i );
        const QVariant property_value = other.getProperty( property_name );
        this->setProperty( property_name , property_value );
    }*/
}

bool GWSObject::incrementProperty(const QString name, const QJsonValue &value){
    QJsonValue existing_value = this->getProperty( name );
    QJsonValue values_sum = GWSObjectFactory::incrementValue( existing_value , value );
    return this->setProperty( name , values_sum );
}

/**********************************************************************
 EVENTS
**********************************************************************/

bool GWSObject::event(QEvent *event){

    if( event->type() == QEvent::DynamicPropertyChange ){
        QDynamicPropertyChangeEvent* const ev = static_cast<QDynamicPropertyChangeEvent*>( event );
        if( ev ){
            QByteArray data = ev->propertyName();
            if( data.length() > 0 ){
                QString property_name = QString( data );
                emit propertyChangedSignal( property_name );
            }
        }
    }

    return QObject::event( event );
}
