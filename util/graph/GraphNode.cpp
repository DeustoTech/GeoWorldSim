#include "GraphNode.h"

#include "../../util/graph/GraphEdge.h"

GWSGraphNode::GWSGraphNode( GWSCoordinate coor ) : geos::planargraph::Node( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

const QList<GWSGraphEdge*> GWSGraphNode::getDepartingEdges() const{
    return this->out_edges;
}

const QList<GWSGraphEdge*> GWSGraphNode::getArrivingEdges() const{
    return this->in_edges;
}

const GWSGraphEdge* GWSGraphNode::getDepartingEdgeTo(GWSGraphNode *to) const{
    foreach(GWSGraphEdge* e , this->getDepartingEdges() ){
        if( e->getToNode()->equals( to ) ){
            return e;
        }
    }
    return 0;
}

const GWSGraphEdge* GWSGraphNode::getArrivingEdgeFrom(GWSGraphNode *from) const{
    foreach(GWSGraphEdge* e , this->getArrivingEdges() ){
        if( e->getFromNode()->equals( from ) ){
            return e;
        }
    }
    return 0;
}

const GWSCoordinate GWSGraphNode::getCoordinate() const{
    return GWSCoordinate( this->pt.x , this->pt.y , this->pt.z );
}

size_t GWSGraphNode::getDegree() const{
    size_t degree = this->out_edges.size();

    // Compare the directed opposite edges as one
    foreach( GWSGraphEdge* edge , this->getArrivingEdges() ){
        bool add = true;
        foreach( GWSGraphEdge* edge2 , this->getDepartingEdges() ){
            if( edge2->equalsReversed( edge ) ){
                add = false;
                break;
            }
        }
        if( add ){ degree++; }
    }
    return degree;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGraphNode::connectEdge(GWSGraphEdge *edge){
    if( edge->getFromNode() == this && !this->getDepartingEdges().contains( edge ) ){
        geos::planargraph::Node::addOutEdge( edge );
        this->out_edges.append( edge );
    }
    if( edge->getToNode() == this &&  !this->getArrivingEdges().contains( edge )){
        this->in_edges.append( edge );
    }
}

void GWSGraphNode::disconnectEdge(GWSGraphEdge *edge){
    if( edge->getFromNode() == this ){
        geos::planargraph::Node::getOutEdges()->remove( edge );
        this->out_edges.removeAll( edge );
    }
    if( edge->getToNode() == this ){
        this->in_edges.removeAll( edge );
    }
}

/**********************************************************************
 METHODS
**********************************************************************/

double GWSGraphNode::getCost() const{
    return 0;
}

bool GWSGraphNode::equals(GWSGraphNode* other){
    if( !other ){ return false; }
    return this->getCoordinate() == other->getCoordinate();
}

