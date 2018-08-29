#include "AgentEnvironment.h"

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
    qInfo() << "Agent environment deleted";
}

/***********************************************************************/
// GETTERS
/***********************************************************************/

quint64 GWSAgentEnvironment::getAmount() const{
    return this->environment_agents->getAmount();
}

bool GWSAgentEnvironment::contains(QString class_name) const{
    return this->environment_agents && this->environment_agents->contains( class_name );
}

QSharedPointer<GWSAgent> GWSAgentEnvironment::getRandomByClass(QString class_name){
    return this->getRandomByClass<GWSAgent>( class_name );
}

template <class T> QSharedPointer<T> GWSAgentEnvironment::getRandomByClass( QString class_name ){
    QList<QSharedPointer<T>> all = this->getByClass<T>( class_name );
    return all.at( qrand() % all.size() );
}

QSharedPointer<GWSAgent> GWSAgentEnvironment::getByClassAndId( QString class_name , QString id) const{
    return this->getByClassAndId<GWSAgent>( class_name , id );
}

template <class T> QSharedPointer<T> GWSAgentEnvironment::getByClassAndId( QString class_name , QString id ) const{
    if( this->environment_agents ){
        QSharedPointer<GWSObject> obj = this->environment_agents->getByClassAndId( class_name , id );
        if( !obj.isNull() ){
            try {
                return obj.dynamicCast<T>();
            } catch(...){}

        }
    }
    return 0;
}

QSharedPointer<GWSAgent> GWSAgentEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->getByClassAndName<GWSAgent>( class_name , agent_name );
}

template <class T> QSharedPointer<T> GWSAgentEnvironment::getByClassAndName( QString class_name , QString agent_name) const{
    return this->environment_agents->getByClassAndName( class_name , agent_name ).dynamicCast<T>();
}

QList< QSharedPointer<GWSAgent> > GWSAgentEnvironment::getByClass( QString class_name ) const{
    return this->getByClass<GWSAgent>( class_name );
}

template <class T> QList<QSharedPointer<T> > GWSAgentEnvironment::getByClass( QString class_name ) const{
    QList<QSharedPointer<T> > agents;
    foreach( QSharedPointer<GWSObject> o , this->environment_agents->getByClass( class_name ) ){
        agents.append( o.dynamicCast<T>() );
    }
    return agents;
}

QSharedPointer<GWSAgent> GWSAgentEnvironment::getByName( QString agent_name ) const{
    return this->getByName<GWSAgent>( agent_name );
}

template <class T> QSharedPointer<T> GWSAgentEnvironment::getByName(QString agent_name) const{
    return this->environment_agents->getByName( agent_name ).dynamicCast<T>();
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSAgentEnvironment::registerAgent( QSharedPointer<GWSAgent> agent){
    GWSEnvironment::registerAgent( agent );
    this->environment_agents->add( agent );
}

void GWSAgentEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent){
    GWSEnvironment::unregisterAgent( agent );
    this->environment_agents->remove( agent );
}

