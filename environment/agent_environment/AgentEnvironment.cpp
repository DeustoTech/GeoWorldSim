#include "AgentEnvironment.h"

GWSAgentEnvironment* GWSAgentEnvironment::globalInstance(){
    static GWSAgentEnvironment instance;
    return &instance;
}

GWSAgentEnvironment::GWSAgentEnvironment() : GWSEnvironment(){
    this->environment_agents = new GWSObjectStorage( this );
    qInfo() << "Agent environment created";
}

GWSAgentEnvironment::~GWSAgentEnvironment(){
    qInfo() << "Agent environment deleted";
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

quint64 GWSAgentEnvironment::getAmount() const{
    return this->environment_agents->getAmount();
}

GWSAgent* GWSAgentEnvironment::getRandomByClass(QString class_name){
    return this->getRandomByClass<GWSAgent>( class_name );
}

template <class T> T* GWSAgentEnvironment::getRandomByClass( QString class_name ){
    QList<T*> all = this->getByClass<T>( class_name );
    return all.at( qrand() % all.size() );
}

GWSAgent* GWSAgentEnvironment::getByClassAndId( QString class_name , QString id) const{
    return this->getByClassAndId<GWSAgent>( class_name , id );
}

template <class T> T* GWSAgentEnvironment::getByClassAndId( QString class_name , QString id ) const{
    return dynamic_cast<T*>( this->environment_agents->getByClassAndId( class_name , id ) );
}

GWSAgent* GWSAgentEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->getByClassAndName<GWSAgent>( class_name , agent_name );
}

template <class T> T* GWSAgentEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return dynamic_cast<T*>( this->environment_agents->getByClassAndName( class_name , agent_name ) );
}

QList<GWSAgent*> GWSAgentEnvironment::getByClass( QString class_name ) const{
    return this->getByClass<GWSAgent>( class_name );
}

template <class T> QList<T*> GWSAgentEnvironment::getByClass( QString class_name ) const{
    QList<T*> agents;
    foreach( GWSObject* o , this->environment_agents->getByClass( class_name ) ){
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

