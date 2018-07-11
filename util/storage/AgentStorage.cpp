#include "AgentStorage.h"

#include "agents/Agent.h"

AgentStorage::AgentStorage(QObject *parent) : GSSObjectStorage( parent ){

}

/**********************************************************************
 GETTERS
**********************************************************************/

Agent* AgentStorage::getByClassAndId(QString class_name, unsigned int id) const{
    if ( this->agents_memory_keys.contains( class_name ) ){
         return this->agents_memory[ class_name ].value( id , 0 );
    }
    return 0;
}

Agent* AgentStorage::getById(unsigned int id) const{
    foreach(QString class_name, this->agents_memory_keys){
        if( this->agents_memory[ class_name ].contains( id ) ){
            return this->agents_memory[ class_name ].value( id , 0 );
        }
    }
    return 0;
}

Agent* AgentStorage::getByClassAndName(QString class_name, QString name) const{
    return dynamic_cast<Agent*>( GSSObjectStorage::getByClassAndName( class_name , name ) );
}

Agent* AgentStorage::getByName(QString name) const{
    return dynamic_cast<Agent*>( GSSObjectStorage::getByName( name ) );
}

QList<Agent*> AgentStorage::getAgentsByClass(QString class_name) const{
    QList<Agent*> agents;
    foreach(QObject* o , GSSObjectStorage::getByClass( class_name )){
        if( dynamic_cast<Agent*>( o ) ){
            agents.append( dynamic_cast<Agent*>( o ) );
        }
    }
    return agents;
}

QList<Agent*> AgentStorage::getAll() const{
    return this->agents_memory[ Agent::staticMetaObject.className() ].values();
}

bool AgentStorage::contains(Agent *agent) const{
    return this->agents_memory_keys.contains( agent->metaObject()->className() ) && this->agents_memory[ agent->metaObject()->className() ].keys().contains( agent->getId() );
}


/**********************************************************************
 SETTERS
**********************************************************************/

void AgentStorage::add( Agent* agent ){

    // Create storages
    QStringList classes = agent->getInheritanceTree();
    foreach( QString c , classes ){
        if( !this->agents_memory_keys.contains( c ) ){
            QList< QObject* > list;
            this->objects.insert( c , list );

            QHash<QString , QObject* > map1;
            this->object_names.insert( c , map1 );

            QHash<unsigned int , Agent*> map2;
            this->agents_memory.insert( c , map2 );

            this->agents_memory_keys.append( c );
        }
    }

    // Add to storage
    foreach( QString c , classes ){
        this->mutex.lock();

        this->objects[ c ].append( agent );
        this->object_names[ c ].insert( agent->objectName() , agent );
        this->agents_memory[ c ].insert( agent->getId() , agent );

        this->mutex.unlock();
    }
}

void AgentStorage::remove(Agent* agent){

    // Remove from storage
    foreach(QString c , agent->getInheritanceTree()){
        this->objects[ c ].removeAll( agent );
        this->object_names[ c ].remove( agent->objectName() );
        this->agents_memory[ c ].remove( agent->getId() );
    }
}
