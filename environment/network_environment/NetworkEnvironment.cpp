#include "NetworkEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"

QString GWSNetworkEnvironment::EDGE_PROP = "edge";

GWSNetworkEnvironment* GWSNetworkEnvironment::globalInstance(){
    static GWSNetworkEnvironment instance;
    return &instance;
}

GWSNetworkEnvironment::GWSNetworkEnvironment() : GWSEnvironment(){
    this->edges_index = new GWSQuadtree();
    qInfo() << "NetworkEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSNetworkEnvironment::~GWSNetworkEnvironment(){
    qDeleteAll( this->network_graphs.values() );
    this->edges_index->deleteLater();
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

/*const QSharedPointer<GWSGraphNode> GWSNetworkEnvironment::getNode(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_node.value( agent );
}*/

const QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getEdge(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.value( agent );
}

const QSharedPointer<GWSAgent> GWSNetworkEnvironment::getAgent(QSharedPointer<GWSGraphEdge> edge) const{
    if( edge.isNull() ){
        return Q_NULLPTR;
    }
    return this->agent_to_edge.key( edge );
}

/*QSharedPointer<GWSGraphNode> GWSNetworkEnvironment::getNodeFromGraph( GWSCoordinate point, QString class_name) const{
    return this->getNodeFromGraph<GWSGraphNode>( point , class_name );
}

template <class T> QSharedPointer<T> GWSNetworkEnvironment::getNodeFromGraph( GWSCoordinate point , QString class_name ) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name )->findNode( point ).dynamicCast<T>();
}*/

QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getNearestEdgeFromGraph( GWSCoordinate coor, QString class_name) const{
    if( !this->network_graphs.keys().contains( class_name ) ){
        return 0;
    }
    return this->network_graphs.value( class_name )->findNearestEdge( coor );
}

QSharedPointer<GWSGraphEdge> GWSNetworkEnvironment::getEdgeFromGraph( GWSCoordinate from,  GWSCoordinate to, QString class_name) const{
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

void GWSNetworkEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){

    if( agent.isNull() ){
        return;
    }

    // GRAPH EDGE (comes parsed by GWSObject, extract and set it to null)
    QSharedPointer<GWSGraphEdge> edge = agent->getProperty( EDGE_PROP ).value< QSharedPointer<GWSObject> >().dynamicCast<GWSGraphEdge>();
    if( edge.isNull() ){
        this->unregisterAgent( agent );
        return;
    }

    // Set to null
    agent->setProperty( EDGE_PROP , QVariant() );

    if( !edge.isNull() ){

        GWSEnvironment::registerAgent( agent );
        QStringList classes = agent->getInheritanceFamily();
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
            if( !edge.isNull() ){
                this->network_graphs.value( c )->addEdge( edge );
                this->agent_to_edge.insert( agent , edge );
                this->edges_index->upsert( edge , edge->getFrom() );
            }
        }
    }
}

void GWSNetworkEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

    QSharedPointer<GWSGraphEdge> edge = agent->getProperty( EDGE_PROP ).value< QSharedPointer<GWSObject> >().dynamicCast<GWSGraphEdge>();

    if( !edge.isNull() ){

        GWSEnvironment::unregisterAgent( agent );
        QStringList classes = agent->getInheritanceFamily();
        foreach(QString c , classes){

            // Remove from spatial graph
            if( !edge.isNull() ){
                this->network_graphs.value( c )->removeEdge( edge );
                this->agent_to_edge.remove( agent );
                this->edges_index->remove( edge );
            }
        }
    }
}
