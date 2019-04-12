#include "NetworkEnvironment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../object/ObjectFactory.h"

#include "../../app/App.h"

QString GWSNetworkEnvironment::EDGE_PROP = "edge";

GWSNetworkEnvironment* GWSNetworkEnvironment::globalInstance(){
    static GWSNetworkEnvironment instance;
    return &instance;
}

GWSNetworkEnvironment::GWSNetworkEnvironment() : GWSEnvironment(){
    qInfo() << "NetworkEnvironment created";
    GWSEnvironmentsGroup::globalInstance()->addEnvironment( this );
}

GWSNetworkEnvironment::~GWSNetworkEnvironment(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSNetworkEnvironment::serialize() const{
    QJsonObject json;
    //QJsonObject json = GWSEnvironment::serializeMini();
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

GWSNetworkEdge GWSNetworkEnvironment::getEdge(QSharedPointer<GWSAgent> agent) const{
    if( agent.isNull() ){
        return GWSNetworkEdge();
    }
    return this->agent_to_edge.value( agent );
}

QSharedPointer<GWSAgent> GWSNetworkEnvironment::getAgent(GWSNetworkEdge edge) const{
    return this->agent_to_edge.key( edge );
}

GWSNetworkEdge GWSNetworkEnvironment::getEdge( GWSCoordinate from ,  GWSCoordinate to, QString class_name) const{
    GWSNetworkEdge edge = this->getNearestEdge( from , class_name );
    if( edge.getToCoordinate() == to ){
        return edge;
    }
    return GWSNetworkEdge();
}

GWSNetworkEdge GWSNetworkEnvironment::getNearestEdge( GWSCoordinate coor , QString class_name ) const{
    if( this->network_edges.keys().contains( class_name ) ){
        return this->edge_ids.value( this->network_edges.value( class_name )->getNearestElement( coor ) );
    }
    return GWSNetworkEdge();
}

QPair< GWSCoordinate , QList< GWSNetworkEdge > > GWSNetworkEnvironment::getNearestNodeAndPath(GWSCoordinate coor, QList<GWSCoordinate> get_nearest, QString class_name) const {
    QList< QList< GWSNetworkEdge > > routes_to_all = this->getShortestPaths( coor , get_nearest , class_name );

    Q_ASSERT( routes_to_all.size() == get_nearest.size() );

    // Check which route is shortest
    GWSLengthUnit shortest_length = GWSLengthUnit( 9999999999 );
    QPair< GWSCoordinate , QList< GWSNetworkEdge > > nearest_node_and_route;
    for(int i = 0 ; i < get_nearest.size() ; i++ ){

        QList< GWSNetworkEdge > route = routes_to_all.at( i );
        if( route.isEmpty() ){ continue; }

        GWSLengthUnit l = GWSLengthUnit(0);
        foreach(GWSNetworkEdge e , route ){
            l = l + e.getLength();
        }
        if( l < shortest_length ){
            shortest_length = l;
            nearest_node_and_route = QPair< GWSCoordinate , QList< GWSNetworkEdge > >( get_nearest.at( i ) , route );
        }
    }
    return nearest_node_and_route;
}

QList<GWSNetworkEdge> GWSNetworkEnvironment::getShortestPath( GWSCoordinate from, GWSCoordinate to , QString class_name ) const{
    QList<GWSNetworkEdge> path;

    if( this->network_routings.keys().contains( class_name ) ){
        // Move given coordinates to real graph nodes
        QString snapped_from = this->getNearestNodeUID( from , class_name );
        QString snapped_to = this->getNearestNodeUID( to , class_name );
        foreach( GWSNetworkEdge e , this->network_routings.value( class_name )->getShortestPath( snapped_from , snapped_to ) ){
            path.append( e );
        }
        return path;
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return path;
}

QList< QList< GWSNetworkEdge > > GWSNetworkEnvironment::getShortestPath( QList< GWSCoordinate > ordered_coors , QString class_name ) const{
    QList< QList< GWSNetworkEdge > > paths;

    if( this->network_routings.keys().contains( class_name ) ){
        QStringList snapped_ordered;
        foreach (GWSCoordinate c, ordered_coors) {
            snapped_ordered.append( this->getNearestNodeUID( c , class_name ) );
        }
        paths = this->network_routings.value( class_name )->getShortestPath( snapped_ordered );
        return paths;
    }

    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return paths;
}

QList< QList< GWSNetworkEdge > > GWSNetworkEnvironment::getShortestPaths( GWSCoordinate from_one, QList< GWSCoordinate > to_many , QString class_name ) const{
    QList< QList< GWSNetworkEdge > > paths;

    if( this->network_routings.keys().contains( class_name ) ){

        QString snapped_from = this->getNearestNodeUID( from_one , class_name );
        if( snapped_from.isEmpty() ){
            for(int i = 0; i < to_many.size() ; i++){
                paths.append( QList<GWSNetworkEdge>() );
            }
            return paths;
        }

        QStringList snapped_to_many;
        foreach( GWSCoordinate c , to_many ) {
            snapped_to_many.append( this->getNearestNodeUID( c , class_name ) );
        }
        paths = this->network_routings.value( class_name )->getShortestPaths( snapped_from , snapped_to_many );
        return paths;

    }
    qWarning() << QString("Asked for path in network %1 which does not exist.").arg( class_name );
    return paths;
}


/**********************************************************************
 SETTERS
**********************************************************************/



/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSNetworkEnvironment::registerAgent( QSharedPointer<GWSAgent> agent ){

    // If already registered
    if( agent.isNull() || agent->getEnvironments().contains( this ) || agent->getProperty( EDGE_PROP ).isNull() ){
        return;
    }

    // GRAPH EDGE (comes as a QJSONOBJECT, need to extract it and build the GWSGRAPHEDGE)
    QJsonObject edge_json = agent->getProperty( EDGE_PROP ).toObject();
    GWSNetworkEdge edge = GWSNetworkEdge( edge_json );
    if( edge.isValid() ){

        this->edge_ids.insert( edge.getUID() , edge );

        try {

            GWSEnvironment::registerAgent( agent );
            QJsonArray classes = agent->getInheritanceFamily();
            QStringList keys = this->network_edges.keys();

            foreach(QJsonValue v , classes){

                QString family = v.toString();
                if( family.isEmpty() ){ continue; }

                // Insert new spatial graph with the agents class
                if( !keys.contains( family ) ){

                    this->mutex.lockForWrite();
                    this->network_edges.insert( family , QSharedPointer< GWSQuadtree >( new GWSQuadtree() ) );
                    this->network_routings.insert( family , QSharedPointer< GWSRouting<GWSNetworkEdge> >( new GWSRouting<GWSNetworkEdge>() ) );
                    this->mutex.unlock();
                }
            }

            foreach(QJsonValue v , classes){

                QString family = v.toString();
                if( family.isEmpty() ){ continue; }

                // Add to spatial graph
                this->network_edges.value( family )->upsert( edge.getUID() , edge.getFromCoordinate() );
                this->network_routings.value( family )->upsert( edge );
                this->agent_to_edge.insert( agent , edge );
            }

        } catch (std::exception &e){
            qWarning() << QString("Simulation threw an exception registering agent to GWSNetworkEnvironment. Exception %1").arg( e.what() );
        }
    }
}

void GWSNetworkEnvironment::unregisterAgent( QSharedPointer<GWSAgent> agent ){

    try {

        GWSEnvironment::unregisterAgent( agent );
        QJsonArray classes = agent->getInheritanceFamily();

        GWSNetworkEdge edge = this->agent_to_edge.value( agent );
        this->edge_ids.remove( edge.getUID() );

        foreach(QJsonValue v , classes){

            QString family = v.toString();
            if( family.isEmpty() ){ continue; }

            // Remove from spatial graph
            if( edge.isValid() ){
                this->network_edges.value( family )->remove( edge.getUID() );
                this->network_routings.value( family )->remove( edge );
                this->agent_to_edge.remove( agent );
            }
        }

    } catch (std::exception &e){
        qWarning() << QString("Simulation threw an exception unregistering agent from GWSNetworkEnvironment. Exception %1").arg( e.what() );
    }
}

/**********************************************************************
 PRIVATE
**********************************************************************/

QString GWSNetworkEnvironment::getNearestNodeUID( GWSCoordinate coor , QString class_name ) const{
    GWSNetworkEdge edge = this->getNearestEdge( coor , class_name );
    //qDebug() << edge.getUID();
    if( !edge.isValid() ){
        return QString();
    }
    GWSLengthUnit from_distance = coor.getDistance( edge.getFromCoordinate() );
    GWSLengthUnit to_distance = coor.getDistance( edge.getToCoordinate() );
    return from_distance < to_distance ? edge.getFromNodeUID() : edge.getToNodeUID();
}

