#include "AgentEnvironment.h"

#include <QPainter>

#include "environment/physical_environment/PhysicalEnvironment.h"

GWSAgentEnvironment* GWSAgentEnvironment::globalInstance(){
    static GWSAgentEnvironment instance;
    return &instance;
}

GWSAgentEnvironment::GWSAgentEnvironment() : GWSEnvironment(){
    this->environment_agents = new GWSObjectStorage();
    qInfo() << "Agent environment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSAgentEnvironment::~GWSAgentEnvironment(){
    this->environment_agents->deleteLater();
    qInfo() << "Agent environment deleted";
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSAgentEnvironment::serialize(){
    QJsonObject json = GWSObject::serialize();
    QJsonArray agent_classes;
    foreach ( QString class_name, this->environment_agents->getClasses() ) {
        agent_classes.append( class_name );
    }
    json.insert( "agent_types" , agent_classes );
    return json;
}

/**
 * @brief Agent::toImage
 * @return
 */
QImage GWSAgentEnvironment::toImage( unsigned int image_width, unsigned int image_height, QStringList class_names){

    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*QPainter painter( &image );

    foreach( QString class_name , class_names ) {
        foreach( GWSAgent* a , this->getByClass( class_name ) ){
            painter.drawImage( 0 , 0 , a->toImage( GWSPhysicalEnvironment::globalInstance()->getBounds() , image_width , image_height ) );
        }
    }*/
    return image;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgentEnvironment::deserialize(QJsonObject json){
    /*GSSAgent::fromJSON(json);
    QJsonObject properties = json["properties"].toObject();
    if( properties.contains( "time" ) ){
        this->time_environment->fromJSON( properties["time"].toObject() );
    }*/
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

quint64 GWSAgentEnvironment::getAmount() const{
    return this->environment_agents->getAmount();
}

GWSAgent* GWSAgentEnvironment::getByClassAndId( const QMetaObject metaobject , QString id) const{
    return this->getByClassAndId<GWSAgent>( metaobject , id );
}

template <class T> T* GWSAgentEnvironment::getByClassAndId( const QMetaObject metaobject , QString id ) const{
    return dynamic_cast<T*>( this->environment_agents->getByClassAndId( metaobject , id ) );
}

GWSAgent* GWSAgentEnvironment::getByClassAndName( const QMetaObject metaobject , QString agent_name) const{
    return this->getByClassAndName<GWSAgent>( metaobject , agent_name );
}

template <class T> T* GWSAgentEnvironment::getByClassAndName( const QMetaObject metaobject , QString agent_name) const{
    return dynamic_cast<T*>( this->environment_agents->getByClassAndName( metaobject , agent_name ) );
}

QList<GWSAgent*> GWSAgentEnvironment::getByClass( const QMetaObject metaobject ) const{
    return this->getByClass<GWSAgent>( metaobject );
}

template <class T> QList<T*> GWSAgentEnvironment::getByClass( const QMetaObject metaobject ) const{
    QList<T*> agents;
    foreach( GWSObject* o , this->environment_agents->getByClass( metaobject ) ){
        agents.append( dynamic_cast<T*>( o ) );
    }
    return agents;
}

GWSAgent* GWSAgentEnvironment::getByName( QString agent_name ) const{
    return this->getByName<GWSAgent>( agent_name );
}

template <class T> T* GWSAgentEnvironment::getByName(QString agent_name) const{
    return dynamic_cast<T*>( this->environment_agents->getByName( agent_name ) );
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSAgentEnvironment::registerAgent(GWSAgent *agent){
    this->environment_agents->add( agent );
}

void GWSAgentEnvironment::unregisterAgent(GWSAgent *agent){
    this->environment_agents->remove( agent );
}

