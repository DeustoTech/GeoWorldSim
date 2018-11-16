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
        json.insert( property_name , this->getProperty( property_name ) );
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
    if( json.keys().contains( GWS_INHERITANCE_FAMILY_PROP ) ){ this->setProperty( GWS_INHERITANCE_FAMILY_PROP , json.value( GWS_INHERITANCE_FAMILY_PROP ).toArray() ); }

    // Set properties
    foreach( QString property_name , json.keys() ){
        // Avoid @ starting keywords
        if( property_name.contains('@') ){ continue; }
        this->setProperty( property_name , json.value( property_name ) );
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

QJsonArray GWSObject::getInheritanceFamily() const{
    QJsonArray inheritance_family;
    inheritance_family.append( this->metaObject()->className() );
    const QMetaObject* mobj = this->metaObject();
    while( mobj ){
        if( !inheritance_family.contains( mobj->className() ) ){
            inheritance_family.append( mobj->className() );
        }
        mobj = mobj->superClass();
    }
    foreach(QJsonValue s , this->getProperty( GWS_INHERITANCE_FAMILY_PROP ).toArray() ){
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
    const QVariant variant = QObject::property( name.toLatin1() );
    this->mutex.unlock();
    return QJsonValue::fromVariant( variant );
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
