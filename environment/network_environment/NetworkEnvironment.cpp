#include "NetworkEnvironment.h"

GWSNetworkEnvironment* GWSNetworkEnvironment::globalInstance(){
    static GWSNetworkEnvironment instance;
    return &instance;
}

GWSNetworkEnvironment::GWSNetworkEnvironment() : GWSEnvironment(){
    qInfo() << "NetworkEnvironment created";
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

GWSGraphNode* GWSNetworkEnvironment::getNodeFromGraph( GeoCoordinates coor, QString class_name) const{
    return this->getNodeFromGraph<GWSGraphNode>( coor , class_name );
}

template <class T> T* GWSNetworkEnvironment::getNodeFromGraph( GeoCoordinates coor , QString class_name ) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return dynamic_cast<T*>( this->network_graphs.value( class_name )->findNode( coor ) );
}

GWSGraphNode* GWSNetworkEnvironment::getNearestNodeFromGraph( GeoCoordinates coor, QString class_name) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name )->findNearestNode( coor );
}

const GWSGraphEdge* GWSNetworkEnvironment::getEdgeFromGraph( GeoCoordinates from,  GeoCoordinates to, QString class_name) const{
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

        QStringList classes = agent->getInheritanceTree();
        QList<QString> keys = this->network_graphs.keys();

        foreach(QString c , classes){

            // Insert new spatial graph with the agents class
            if( !keys.contains( c ) ){

                this->mutex.lock();
                this->network_graphs.insert( c , new GWSGraph() );
                this->mutex.unlock();

            }
        }

        foreach(QString c , classes){

            // Add to spatial graph
            if( edge ){
                this->network_graphs.value( c )->addEdge( edge );
            }
            if( node ){
                this->network_graphs.value( c )->addNode( node );
            }
        }
    }
}

void GWSNetworkEnvironment::unregisterAgent( GWSAgent *agent ){
    GWSGraphEdge* edge = dynamic_cast<GWSGraphEdge*>( agent );
    GWSGraphNode* node = dynamic_cast<GWSGraphNode*>( agent );

    if( edge || node ){

        QStringList classes = agent->getInheritanceTree();
        foreach(QString c , classes){

            // Remove from spatial graph
            if( edge ){
                this->network_graphs.value( c )->removeEdge( edge );
            }
            if( node){
                this->network_graphs.value( c )->removeNode( node );
            }
        }
    }
}
