#include "GraphNode.h"

#include "../../util/graph/GraphEdge.h"

GWSGraphNode::GWSGraphNode() : GWSObject(){
}

GWSGraphNode::~GWSGraphNode(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGraphNode::deserialize(QJsonObject json, QSharedPointer<GWSObject> parent){
    GWSObject::deserialize( json , parent );
    double x = json.value( "x" ).toDouble();
    double y = json.value( "y" ).toDouble();
    this->inner_coordinate = GWSCoordinate( x , y );
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSGraphNode::serialize() const{
    QJsonObject json = GWSObject::serialize();
    json.insert( "x" , this->inner_coordinate.getX() );
    json.insert( "y" , this->inner_coordinate.getY() );
    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

const QList< QSharedPointer<GWSGraphEdge> > GWSGraphNode::getDepartingEdges() const{
    return this->out_edges;
}

const QList< QSharedPointer<GWSGraphEdge> > GWSGraphNode::getArrivingEdges() const{
    return this->in_edges;
}

const QSharedPointer<GWSGraphEdge> GWSGraphNode::getDepartingEdgeTo(QSharedPointer<GWSGraphNode> to) const{
    foreach(QSharedPointer<GWSGraphEdge> e , this->getDepartingEdges() ){
        if( e->getToNode()->equals( to ) ){
            return e;
        }
    }
    return 0;
}

const QSharedPointer<GWSGraphEdge> GWSGraphNode::getArrivingEdgeFrom(QSharedPointer<GWSGraphNode> from) const{
    foreach(QSharedPointer<GWSGraphEdge> e , this->getArrivingEdges() ){
        if( e->getFromNode()->equals( from ) ){
            return e;
        }
    }
    return 0;
}

int GWSGraphNode::getDegree() const{
    int degree = this->out_edges.size();

    // Compare the directed opposite edges as one
    foreach( QSharedPointer<GWSGraphEdge> edge , this->getArrivingEdges() ){
        bool add = true;
        foreach( QSharedPointer<GWSGraphEdge> edge2 , this->getDepartingEdges() ){
            if( edge2->equalsReversed( edge ) ){
                add = false;
                break;
            }
        }
        if( add ){ degree++; }
    }
    return degree;
}

GWSCoordinate GWSGraphNode::getCoordinate(){
    return this->inner_coordinate;
}

/**********************************************************************
 PRIVATE
**********************************************************************/

void GWSGraphNode::connectEdge(QSharedPointer<GWSGraphEdge> edge){
    if( edge->getFromNode() == this && !this->getDepartingEdges().contains( edge ) ){
        //geos::planargraph::Node::addOutEdge( edge );
        this->out_edges.append( edge );
    }
    if( edge->getToNode() == this &&  !this->getArrivingEdges().contains( edge )){
        this->in_edges.append( edge );
    }
}

void GWSGraphNode::disconnectEdge(QSharedPointer<GWSGraphEdge> edge){
    if( edge->getFromNode() == this ){
        //geos::planargraph::Node::getOutEdges()->remove( edge );
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

bool GWSGraphNode::equals(QSharedPointer<GWSGraphNode> other){
    if( !other ){ return false; }
    return this->inner_coordinate == other->inner_coordinate;
}

