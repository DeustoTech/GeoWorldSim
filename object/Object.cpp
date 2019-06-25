#include "Object.h"

#include <QDebug>
#include <QDateTime>
#include <QEvent>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrent>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

QString GWSObject::GWS_UID_PROP = "id";
QString GWSObject::GWS_CLASS_PROP = "@gwsclass";
QString GWSObject::GWS_INHERITANCE_FAMILY_PROP = "@gwsgroups";

quint64 GWSObject::counter = QDateTime::currentMSecsSinceEpoch();

GWSObject::GWSObject() : QObject() , deleted(false) {
    this->properties = new QJsonObject();

    QString generated_id = QString("Entity::%1").arg( ++GWSObject::counter );
    this->setProperty( GWSObject::GWS_UID_PROP ,  generated_id );
}

GWSObject::GWSObject(const GWSObject &other) : QObject(){
}

GWSObject::~GWSObject(){
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
    QJsonObject json( this->serializeMini() );

    this->mutex.lockForRead();
    QStringList keys = this->properties->keys();
    this->mutex.unlock();

    foreach(QString property_name , keys ) {
        if ( property_name.startsWith( "@" ) ){ continue; }
        json.insert( property_name , GWSObject::getProperty( property_name ) ); 
    }
    return json;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSObject::deserialize(const QJsonObject &json, QSharedPointer<GWSObject> parent){
    Q_UNUSED( parent );

    // Required properties
    /*QStringList parsed_properties;
    if( json.keys().contains( GWS_UID_PROP ) ){         parsed_properties.append( GWS_UID_PROP ); this->setProperty( GWS_UID_PROP , json.value( GWS_UID_PROP ).toString() ); }
    if( json.keys().contains( GWS_CLASS_PROP ) ){       parsed_properties.append( GWS_CLASS_PROP ); this->setProperty( GWS_CLASS_PROP , json.value( GWS_CLASS_PROP ).toString() ); }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) && json.value( GWS_INHERITANCE_FAMILY_PROP ).isString() ){
        parsed_properties.append( GWS_INHERITANCE_FAMILY_PROP);
        QJsonArray arr;
        arr.append( json.value( GWS_INHERITANCE_FAMILY_PROP ).toString() );
        this->setProperty( GWS_INHERITANCE_FAMILY_PROP , arr );
    }
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) && json.value( GWS_INHERITANCE_FAMILY_PROP ).isArray() ){
        parsed_properties.append( GWS_INHERITANCE_FAMILY_PROP );
        this->setProperty( GWS_INHERITANCE_FAMILY_PROP , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray() );
    }*/

    // Remaining properties
    foreach( QString property_name , json.keys() ){

        /*if( parsed_properties.contains( property_name ) ){
            continue; // Do not overwrite just parsed properties
        }*/

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

bool GWSObject::hasProperty( const QString &name ) const{
    //return !QObject::property( name.toUtf8() ).isNull();
    this->mutex.lockForRead();
    bool contains = this->properties->keys().contains( name );
    this->mutex.unlock();
    return contains;
}

QJsonValue GWSObject::getProperty( const QString &name ) const{
    return this->getProperties( QStringList({ name }) ).value( name );
}

QJsonObject GWSObject::getProperties(const QStringList &names) const{
    QJsonObject props;

    this->mutex.lockForRead();
    foreach (QString name , names) {

        // If it comes between '<>', it is not the property name, but a key where to get the property name from
        if( name.startsWith( "<" ) && name.endsWith( ">" ) ){
            QString property_name = name;
            property_name = property_name.remove( 0 , 1 );
            property_name = property_name.remove( property_name.length() - 1 , 1 );
            property_name = this->properties->value( property_name ).toString();

            QJsonValue v = this->properties->value( property_name );
            props.insert( property_name , v.isUndefined() || v.isNull() ? QJsonValue::Null : v );
        }
        else
        {
            QJsonValue v = this->properties->value( name );
            props.insert( name , v.isUndefined() || v.isNull() ? QJsonValue::Null : v );
        }

    }
    this->mutex.unlock();

    return props;
}

QJsonValue GWSObject::operator []( const QString &name ) const{
    return this->getProperty( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

bool GWSObject::setProperty( const QString &name, const GWSUnit &value){
    return this->setProperties( QJsonObject({ { name , value.number() } }) );
}

bool GWSObject::setProperty( const QString &name, const QJsonValue &value){
    return this->setProperties( QJsonObject({ { name , value } }) );
}

bool GWSObject::setProperties(const QJsonObject &obj){

    this->mutex.lockForWrite();
    foreach( QString name , obj.keys()) {
        this->properties->insert( name , obj.value( name ) );
    }
    this->mutex.unlock();

    foreach( QString name , obj.keys() ) {
        foreach(std::function<void( QSharedPointer<GWSObject> , QString )> callback , this->subscriptions.value( name ) ){
            QtConcurrent::run([ callback , this , &name ]{
                callback( this->getSharedPointer() , name );
            });
        }
    }

    return true;
}

bool GWSObject::removeProperty( const QString &name ){
    return this->removeProperties( QStringList({ name }) );
}

bool GWSObject::removeProperties(const QStringList &names){

    this->mutex.lockForWrite();
    foreach( QString name , names) {
        this->properties->remove( name );
    }
    this->mutex.unlock();

    foreach( QString name , names ) {
        foreach(std::function<void( QSharedPointer<GWSObject> , QString )> callback , this->subscriptions.value( name ) ){
            QtConcurrent::run([ callback , this , &name ]{
                callback( this->getSharedPointer() , name );
            });
        }
    }

    return true;
}

void GWSObject::copyProperties( GWSObject &other){
    /*for (int i = 0; i < other.dynamicPropertyNames().size(); ++i) {
        const char* property_name = other.dynamicPropertyNames().at( i );
        const QVariant property_value = other.getProperty( property_name );
        this->setProperty( property_name , property_value );
    }*/
}

bool GWSObject::incrementProperty( QString &name, const QJsonValue &value){
    QJsonValue existing_value = this->getProperty( name );
    QJsonValue values_sum = GWSObjectFactory::incrementValue( existing_value , value );
    return this->setProperty( name , values_sum );
}

void GWSObject::addSubscription(const QString &property_name, std::function<void (QSharedPointer<GWSObject>, QString)> callback){

}

/**********************************************************************
 EVENTS
**********************************************************************/

/*bool GWSObject::event(QEvent *event){

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
