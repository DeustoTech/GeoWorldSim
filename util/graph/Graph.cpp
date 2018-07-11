#include "Graph.h"

#include <QImage>
#include <QPainter>

#include "../../environment/Environment.h"
#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"
#include "geos/geom/CoordinateArraySequenceFactory.h"
#include "geos/geom/Envelope.h"
#include "../../util/graph/GraphUtils.h"

GWSGraph::GWSGraph() : PlanarGraph(){
    this->nodes_index = new GWSQuadtree();
}

GWSGraph::~GWSGraph() {
    delete this->nodes_index;
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSGraph::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height) const{

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QColor(0,0,0) );

    foreach( GWSGraphEdge* edge , this->getEdges() ){

        GWSCoordinate coor1 = edge->getFromNode()->getCoordinate();
        QPoint p1 = ImageCoordinatesConversor::reprojectPoint( coor1.getY() , coor1.getX() , image_bounds.getMaxY() , image_bounds.getMinY() , image_bounds.getMaxX() , image_bounds.getMinX() , image_width , image_height );
        GWSCoordinate coor2 = edge->getToNode()->getCoordinate();
        QPoint p2 = ImageCoordinatesConversor::reprojectPoint( coor2.getY() , coor2.getX() , image_bounds.getMaxY() , image_bounds.getMinY() , image_bounds.getMaxX() , image_bounds.getMinX() , image_width , image_height );
        painter.drawLine( p1 , p2 );

    }*/
    return image;
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSGraph::containsNode( GWSGraphNode* node ) const{
    GWSGraphNode* found = this->findNode( node->getCoordinate() );
    return found == node;
}

bool GWSGraph::containsEdge( GWSGraphEdge* edge) const{
    GWSGraphEdge* found = this->findEdge( edge->getFromNode()->getCoordinate() , edge->getToNode()->getCoordinate() );
    return found == edge;
}

GWSGraphEdge* GWSGraph::findEdge(GWSCoordinate from, GWSCoordinate to) const{
    if( from == to ){
        return 0;
    }
    GWSGraphNode* from_node = this->findNode( from );
    GWSGraphNode* to_node = this->findNode( to );
    if( !from_node || !to_node ){
        return 0;
    }
    for(unsigned int i = 0; i < this->dirEdges.size(); i++){
        GWSGraphEdge* edge = dynamic_cast<GWSGraphEdge*>( this->dirEdges.at(i) );
        if( edge->getFromNode() == from_node && edge->getToNode() == to_node ){
            return edge;
        }
    }
    return 0;
}

GWSGraphNode* GWSGraph::findNode(GWSCoordinate coor) const{
    geos::planargraph::Node* n = const_cast<GWSGraph*>( this )->PlanarGraph::findNode( geos::geom::Coordinate( coor.getX() , coor.getY() , coor.getZ() ) );
    if( n ){
        return dynamic_cast<GWSGraphNode*>( n );
    }
    return 0;
}

/** NOTICE that first of all it will call the FindNode method
 * @brief Graph::findNearestNode
 * @param coor
 * @return
 */
GWSGraphNode* GWSGraph::findNearestNode(GWSCoordinate coor) const{
    GWSGraphNode* found = this->findNode( coor );
    if( !found && this->nodes_index ){
        return GWSGraphUtils::globalInstance()->findNearestFromQuadtree( coor , this->nodes_index );
    }
    return found;
}

QList<GWSGraphEdge*> GWSGraph::getEdges() const{
    QList<GWSGraphEdge*> edges;
    for(unsigned int i = 0; i < this->dirEdges.size(); i++){
        edges.append( dynamic_cast<GWSGraphEdge*>( this->dirEdges.at(i) ) );
    }
    return edges;
}

QList<GWSGraphEdge*> GWSGraph::getEdgesInEnvelope(GWSEnvelope env) const{
    QList<GWSGraphEdge*> edges;
    for(unsigned int i = 0; i < this->dirEdges.size(); i++){
        geos::planargraph::DirectedEdge* e  = dynamic_cast<geos::planargraph::DirectedEdge*>( this->dirEdges.at(i) );
        geos::geom::Coordinate c2 = e->getToNode()->getCoordinate();
        geos::geom::Coordinate c1 = e->getFromNode()->getCoordinate();
        if( env.contains( GWSCoordinate( c1.x , c1.y , c1.z ) ) && env.contains( GWSCoordinate( c2.x , c2.y , c2.z ) ) ){
            edges.append( dynamic_cast<GWSGraphEdge*>( this->dirEdges.at(i) )  );
        }
    }
    return edges;
}

QList<GWSGraphNode*> GWSGraph::getNodes() const{
    QList<GWSGraphNode*> nodes;
    std::vector<geos::planargraph::Node*> ns;
    const_cast<GWSGraph*>( this )->PlanarGraph::getNodes( ns );
    for(unsigned int i = 0; i < ns.size(); i++){
        nodes.append( dynamic_cast<GWSGraphNode*>( ns.at(i) ) );
    }
    return nodes;
}

QMap<GWSGraphEdge*,double> GWSGraph::getCostMap() const{
    QMap<GWSGraphEdge*,double> cost_map;
    foreach( GWSGraphEdge* edge , this->getEdges() ){
        cost_map.insert( edge , edge->getCost() );
    }
    return cost_map;
}

QList<GWSGraphNode*> GWSGraph::findNodesOfDegree( int degree ) const{
    QList<GWSGraphNode*> nodes;
    std::vector<geos::planargraph::Node*> ns;
    const_cast<GWSGraph*>( this )->PlanarGraph::findNodesOfDegree( degree , ns );
    for(unsigned int i = 0; i < ns.size(); i++){
        nodes.append( dynamic_cast<GWSGraphNode*>( ns.at(i) ) );
    }
    return nodes;
}

int GWSGraph::countNodes() const{
    return this->getNodes().size();
}

int GWSGraph::countEdges() const{
    return this->dirEdges.size();
}

/*MultiLineString* GWSGraph::toMultiLineString() const{
    std::vector<Geometry*> *lines = new std::vector<Geometry*>();
    foreach( GWSGraphEdge* e , this->getEdges() ){
        std::size_t s = 0;
        CoordinateSequence* seq = factory->getCoordinateSequenceFactory()->create( s , s );
        seq->add( geos::geom::Coordinate ( e->getFromNode()->getCoordinate().getX() , e->getFromNode()->getCoordinate().getY() , e->getFromNode()->getCoordinate().getZ() ) );
        seq->add(  geos::geom::Coordinate ( e->getToNode()->getCoordinate().getX() , e->getToNode()->getCoordinate().getY() , e->getToNode()->getCoordinate().getZ() ) );
        geos::geom::Geometry* line = factory->createLineString( seq );
        lines->push_back( line );
    }
    return factory->createMultiLineString( lines );
}*/

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGraph::addGraph(const GWSGraph *other){
    std::vector<geos::planargraph::Node*> ns;
    const_cast<GWSGraph*>( other )->PlanarGraph::getNodes( ns );
    foreach( geos::planargraph::Node* n , ns ){
        GWSGraphNode* node = dynamic_cast<GWSGraphNode*>( n );
        this->addNode( node );
        foreach( GWSGraphEdge* edge , node->getDepartingEdges() ){
            geos::planargraph::PlanarGraph::add( edge );
        }
    }
}

void GWSGraph::addEdge(GWSGraphEdge *edge){
    GWSGraphNode* from = this->findNode( edge->getFromNode()->getCoordinate() );
    if( !from ){
        this->addNode( edge->getFromNode() );
    }
    GWSGraphNode* to = this->findNode( edge->getToNode()->getCoordinate() );
    if( !to ){
        this->addNode( edge->getToNode() );
    }
    geos::planargraph::PlanarGraph::add( edge );
}

void GWSGraph::removeEdge(GWSGraphEdge *edge){
    auto it = std::find( this->dirEdges.begin(), this->dirEdges.end(), edge);
    if(it != this->dirEdges.end()){
        this->dirEdges.erase( it );
    }
    //geos::planargraph::PlanarGraph::remove( edge ); NOT WORKING
}

void GWSGraph::addNode(GWSGraphNode *node){
    GWSEnvelope env = GWSEnvelope( node->getCoordinate().getX() , node->getCoordinate().getX() , node->getCoordinate().getY() , node->getCoordinate().getY() );
    this->nodes_index->insert( env , node );
    geos::planargraph::PlanarGraph::add( node );
}

void GWSGraph::removeNode(GWSGraphNode *node){
    GWSEnvelope env = GWSEnvelope( node->getCoordinate().getX() , node->getCoordinate().getX() , node->getCoordinate().getY() , node->getCoordinate().getY() );
    this->nodes_index->remove( env , node );
    geos::planargraph::PlanarGraph::remove( node );
}

void GWSGraph::replaceNode(GWSGraphNode* old_node , GWSGraphNode *new_node){

    // Take out connected edges
    foreach( GWSGraphEdge* e , old_node->getDepartingEdges() ){
        this->removeEdge( e );
    }
    foreach( GWSGraphEdge* e , old_node->getArrivingEdges() ){
        this->removeEdge( e );
    }

    // Remove old node
    this->removeNode( old_node );
    // Add new node
    this->addNode( new_node );

    // Add again edges with the new node set
    foreach( GWSGraphEdge* e , old_node->getDepartingEdges() ){
        e->from = new_node;
        this->addEdge( e );
    }
    foreach( GWSGraphEdge* e , old_node->getArrivingEdges() ){
        e->to = new_node;
        this->addEdge( e );
    }
}

