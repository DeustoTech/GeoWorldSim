#include "Object.h"

#include <QVector>
#include <QDebug>

#include "../../app/App.h"

#include "../../util/conversors/custom_json/CustomJsonConversor.h"

QString GWSObject::GWS_ID_PROP = "@id";
QString GWSObject::GWS_TYPE_PROP = "@type";

unsigned int GWSObject::counter = 0;

GWSObject::GWSObject( GWSObject *parent ) : QObject( parent ){
    this->setProperty( GWSObject::GWS_ID_PROP , QString("%1-%2%3").arg( GWSApp::globalInstance()->getAppId() ).arg( this->metaObject()->className() ).arg( ++GWSObject::counter ) );
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
        json.insert( property_name , this->property( property_name ).toJsonValue() );
    }
    return json;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSObject::deserialize(QJsonObject json){

    // Set properties
    foreach( QString key , json.keys() ){

        if( key.startsWith('@') ){ continue; } // Avoid control params

        QJsonValue value = json.value( key );
        this->setProperty( key , value );
    }
}


/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSObject::getId() const{
    return this->property( GWSObject::GWS_ID_PROP ).toString();
}

QStringList GWSObject::getInheritanceTree() const{
    QStringList l;
    const QMetaObject* obj = this->metaObject();
    while( obj ){
        l.append( obj->className() );
        obj = obj->superClass();
    }
    return l;
}

const QVariant GWSObject::property( QString name ) const{
    return QObject::property( name.toLatin1() );
}

const QVariant GWSObject::operator []( QString name ) const{
    return this->property( name );
}

/**********************************************************************
 SETTERS
**********************************************************************/


void GWSObject::setProperty( QString name, const QJsonValue &value){
    QObject::setProperty( name.toLatin1() , value );
}
