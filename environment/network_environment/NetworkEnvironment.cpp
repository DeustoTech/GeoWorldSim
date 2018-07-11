#include "NetworkEnvironment.h"

#include "environment/Environment.h"
#include "util/parallelism/ParallelismController.h"


GWSNetworkEnvironment* GWSNetworkEnvironment::globalInstance(){
    static GWSNetworkEnvironment instance;
    return &instance;
}

GWSNetworkEnvironment::GWSNetworkEnvironment() : GWSEnvironment(){
    qInfo() << "NetworkEnvironment created";
    GWSEnvironment::globalInstance()->registerSubenvironment( this );
}

GWSNetworkEnvironment::~GWSNetworkEnvironment(){
    qDeleteAll( this->network_graphs.values() );
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSNetworkEnvironment::serialize() const{
    QJsonObject json;
    return json;
}

void GWSNetworkEnvironment::deserialize(QJsonObject json){
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSGraphNode* GWSNetworkEnvironment::getNodeFromGraph( GWSCoordinate coor, QString class_name) const{
    return this->getNodeFromGraph<GWSGraphNode>( coor , class_name );
}

template <class T> T* GWSNetworkEnvironment::getNodeFromGraph( GWSCoordinate coor , QString class_name ) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return dynamic_cast<T*>( this->network_graphs.value( class_name )->findNode( coor ) );
}

GWSGraphNode* GWSNetworkEnvironment::getNearestNodeFromGraph( GWSCoordinate coor, QString class_name) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name )->findNearestNode( coor );
}

const GWSGraphEdge* GWSNetworkEnvironment::getEdgeFromGraph( GWSCoordinate from,  GWSCoordinate to, QString class_name) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name )->findEdge( from , to );
}

const GWSGraph* GWSNetworkEnvironment::getGraph( QString class_name ) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name );
}

/**********************************************************************
 SETTERS
**********************************************************************/



/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSNetworkEnvironment::registerAgent( GWSAgent *agent ){

    GWSGraphEdge* edge = dynamic_cast<GWSGraphEdge*>( agent );
    GWSGraphNode* node = dynamic_cast<GWSGraphNode*>( agent );

    if( edge || node ){

        const QMetaObject* class_type = agent->metaObject();
        QList<QString> keys = this->network_graphs.keys();
        while( class_type && !keys.contains( class_type->className() ) ){

            // Insert new spatial graph with the agents class
            this->mutex.lock();
            GWSGraph* graph = new GWSGraph();
            this->network_graphs.insert( class_type->className() , graph );
            this->mutex.unlock();

            class_type = class_type->superClass();
        }

        class_type = agent->metaObject();
        while( class_type ){

            // Add to spatial graph
            if( edge ){
                this->network_graphs.value( class_type->className() )->addEdge( edge );
            }
            if( node ){
                this->network_graphs.value( class_type->className() )->addNode( node );
            }

            class_type = class_type->superClass();
        }
    }
}

void GWSNetworkEnvironment::unregisterAgent( GWSAgent *agent ){
    GWSGraphEdge* edge = dynamic_cast<GWSGraphEdge*>( agent );
    GWSGraphNode* node = dynamic_cast<GWSGraphNode*>( agent );

    if( edge || node ){

        const QMetaObject* class_type = agent->metaObject();
        while( class_type ){

            // Add to spatial graph
            if( edge ){
                this->network_graphs.value( class_type->className() )->removeEdge( edge );
            }
            if( node){
                this->network_graphs.value( class_type->className() )->removeNode( node );
            }

            class_type = class_type->superClass();
        }
    }
}
