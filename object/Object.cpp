#include "Object.h"

#include <QDebug>
#include <QDateTime>
#include <QEvent>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrent>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"


QString geoworldsim::Object::GWS_UID_PROP = "id";
QString geoworldsim::Object::GWS_CLASS_PROP = "@gwsclass";
QString geoworldsim::Object::GWS_INHERITANCE_FAMILY_PROP = "@gwsgroups";

quint64 geoworldsim::Object::counter = QDateTime::currentMSecsSinceEpoch();

geoworldsim::Object::Object() : QObject() , deleted(false) {
    this->properties = new QJsonObject();

    QString generated_id = QString("Entity::%1").arg( ++geoworldsim::Object::counter );
    this->setProperty( geoworldsim::Object::GWS_UID_PROP ,  generated_id );
}

geoworldsim::Object::Object(const geoworldsim::Object &other) : QObject(){
    this->properties = other.properties;
}

geoworldsim::Object::~Object(){
    this->beforeDestroyHook();
    this->deleted = true;
    delete this->properties;
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

/**
 * @brief GSSObject::toMiniJSON Called when asked for many objects
 */
QJsonObject geoworldsim::Object::serializeMini() const{
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
QJsonObject geoworldsim::Object::serialize() const{
    QJsonObject json( this->serializeMini() );

    this->mutex.lockForRead();
    foreach( QString key , this->properties->keys() ){
        QJsonValue v = this->properties->value( key );
        json.insert( key , v.isUndefined() || v.isNull() ? QJsonValue::Null : v );
    }
    this->mutex.unlock();

    return json;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void geoworldsim::Object::deserialize(const QJsonObject &json, QSharedPointer<geoworldsim::Object> parent){
    Q_UNUSED( parent );

    // Required properties
    this->properties->insert( GWS_UID_PROP , json.value( GWS_UID_PROP ).toString( this->getUID() ) );
    this->properties->insert( GWS_CLASS_PROP , json.value( GWS_CLASS_PROP ).toString() );
    if( json.value( GWS_INHERITANCE_FAMILY_PROP ).isString() ){
        this->properties->insert( GWS_INHERITANCE_FAMILY_PROP , { json.value( GWS_INHERITANCE_FAMILY_PROP ).toString() } );
    }
    if( json.value( GWS_INHERITANCE_FAMILY_PROP ).isArray() ){
        this->properties->insert( GWS_INHERITANCE_FAMILY_PROP , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray() );
    }

    // Remaining properties
    foreach( QString property_name , json.keys() ){

        if( property_name == GWS_UID_PROP || property_name == GWS_CLASS_PROP || property_name == GWS_INHERITANCE_FAMILY_PROP ){
            continue; // Do not overwrite just parsed properties
        }

        if( property_name.startsWith('@') ){
            continue; // Do not set system properties
        }

        // Get the value to be inserted
        QJsonValue propert_value = json.value( property_name );

        // If is complex JSON (with value, metadata, etc)
        if( propert_value.isObject() && propert_value.toObject().keys().contains("value") ){
            propert_value = propert_value.toObject().value( "value" );
        }

        // Set Property
        this->properties->insert( property_name , propert_value );
    }
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::Object::getUID() const{
    this->mutex.lockForRead();
    QJsonValue uuid = this->properties->value( geoworldsim::Object::GWS_UID_PROP );
    this->mutex.unlock();
    return uuid.toString();
}

QSharedPointer<geoworldsim::Object> geoworldsim::Object::getSharedPointer() const{
    return this->self_shared_pointer;
}

QJsonArray geoworldsim::Object::getInheritanceFamily() const{

    this->mutex.lockForRead();
    QJsonArray inheritance_family = this->properties->value( geoworldsim::Object::GWS_INHERITANCE_FAMILY_PROP ).toArray();
    this->mutex.unlock();

    inheritance_family.prepend( this->metaObject()->className() );
    const QMetaObject* mobj = this->metaObject();
    while( mobj && mobj->className() != QObject::metaObject()->className() ){
        if( !inheritance_family.contains( mobj->className() ) ){
            inheritance_family.prepend( mobj->className() );
        }
        mobj = mobj->superClass();
    }

    return inheritance_family;
}

bool geoworldsim::Object::hasProperty( const QString &name ) const{
    return this->properties->contains( name );
}

QJsonValue geoworldsim::Object::getProperty( const QString &name ) const{
    if( name.startsWith( "<" ) && name.endsWith( ">" ) ){
        QString str = name;
        str = str.remove( 0 , 1 );
        str = str.remove( str.length() - 1 , 1 );
        this->mutex.lockForRead();
        str = this->properties->value( str ).toString();

        QJsonValue v = this->properties->value( str );
        this->mutex.unlock();
        return ( v.isUndefined() || v.isNull() ) ? QJsonValue::Null : v;
    }
    else
    {
        this->mutex.lockForRead();
        QJsonValue v = this->properties->value( name );
        this->mutex.unlock();    
        return ( v.isUndefined() || v.isNull() ) ? QJsonValue::Null : v;
    }
}

QJsonObject geoworldsim::Object::getProperties(const QStringList &names) const{
    QJsonObject props;

    foreach( QString name , names ){

        // If it comes between '<>', it is not the property name, but a key where to get the property name from
        if( name.startsWith( "<" ) && name.endsWith( ">" ) ){
            QString str = name;
            str = str.remove( 0 , 1 );
            str = str.remove( str.length() - 1 , 1 );
            this->mutex.lockForRead();
            str = this->properties->value( str ).toString();

            QJsonValue v = this->properties->value( str );
            this->mutex.unlock();
            props.insert( str , v.isUndefined() || v.isNull() ? QJsonValue::Null : v );
        }
        else
        {
            this->mutex.lockForRead();
            QJsonValue v = this->properties->value( name );
            this->mutex.unlock();
            props.insert( name , v.isUndefined() || v.isNull() ? QJsonValue::Null : v );
        }

    }

    return props;
}

QJsonValue geoworldsim::Object::operator []( const QString &name ) const{
    return this->getProperty( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

bool geoworldsim::Object::setProperty( const QString &name, const geoworldsim::unit::Unit &value){
    return this->setProperties( { { name , value.number() } } );
}

bool geoworldsim::Object::setProperty( const QString &name, const QJsonValue &value){
    return this->setProperties( { { name , value } } );
}

bool geoworldsim::Object::setProperties(const QJsonObject &obj){

    this->mutex.lockForWrite();
    foreach( QString name , obj.keys()) {
        this->properties->insert( name , obj.value( name ) );
    }
    this->mutex.unlock();

    foreach( QString name , obj.keys() ) {
        foreach(std::function<void( QSharedPointer<geoworldsim::Object> , QString )> callback , this->subscriptions.value( name ) ){
            QtConcurrent::run([ callback , this , &name ]{
                callback( this->getSharedPointer() , name );
            });
        }
    }

    return true;
}

bool geoworldsim::Object::removeProperty( const QString &name ){
    return this->removeProperties( QStringList({ name }) );
}

bool geoworldsim::Object::removeProperties(const QStringList &names){

    this->mutex.lockForWrite();
    foreach( QString name , names) {
        this->properties->remove( name );
    }
    this->mutex.unlock();

    foreach( QString name , names ) {
        foreach(std::function<void( QSharedPointer<geoworldsim::Object> , QString )> callback , this->subscriptions.value( name ) ){
            QtConcurrent::run([ callback , this , &name ]{
                callback( this->getSharedPointer() , name );
            });
        }
    }

    return true;
}

void geoworldsim::Object::copyProperties( geoworldsim::Object &other){
    /*for (int i = 0; i < other.dynamicPropertyNames().size(); ++i) {
        const char* property_name = other.dynamicPropertyNames().at( i );
        const QVariant property_value = other.getProperty( property_name );
        this->setProperty( property_name , property_value );
    }*/
}

bool geoworldsim::Object::incrementProperty( QString &name, const QJsonValue &value){
    QJsonValue existing_value = this->getProperty( name );
    QJsonValue values_sum = geoworldsim::ObjectFactory::incrementValue( existing_value , value );
    return this->setProperty( name , values_sum );
}

void geoworldsim::Object::addSubscription(const QString &property_name, std::function<void (QSharedPointer<geoworldsim::Object>, QString)> callback){

}

/**********************************************************************
 EVENTS
**********************************************************************/

/*bool geoworldsim::GWSObject::event(QEvent *event){

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
}*/

