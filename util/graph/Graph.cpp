#include "Graph.h"

GWSGraph::GWSGraph() {
    this->edges_index = new GWSQuadtree();
}

GWSGraph::~GWSGraph() {
    this->edges_index->deleteLater();
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/*QImage GWSGraph::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height) const{

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QColor(0,0,0) );

    foreach( QSharedPointer<GWSGraphEdge> edge , this->getEdges() ){

        GWSCoordinate coor1 = edge->getFromNode()->getCoordinate();
        QPoint p1 = ImageCoordinatesConversor::reprojectPoint( coor1.getY() , coor1.getX() , image_bounds.getMaxY() , image_bounds.getMinY() , image_bounds.getMaxX() , image_bounds.getMinX() , image_width , image_height );
        GWSCoordinate coor2 = edge->getToNode()->getCoordinate();
        QPoint p2 = ImageCoordinatesConversor::reprojectPoint( coor2.getY() , coor2.getX() , image_bounds.getMaxY() , image_bounds.getMinY() , image_bounds.getMaxX() , image_bounds.getMinX() , image_width , image_height );
        painter.drawLine( p1 , p2 );

    }
    return image;
}*/

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSGraph::containsCoordinate( GWSCoordinate coor ) const{
    return this->coordinates.contains( coor );
}

bool GWSGraph::containsEdge( QSharedPointer<GWSGraphEdge> edge) const{
    return this->edges.contains( edge );
}

const QSharedPointer<GWSGraphEdge> GWSGraph::findNearestEdge(GWSCoordinate coor) const{
    return this->edges_index->getNearestElement( coor ).dynamicCast<GWSGraphEdge>();
}

const QSharedPointer<GWSGraphEdge> GWSGraph::findEdge(GWSCoordinate from, GWSCoordinate to) const{
    /*if( from == to ){
        return 0;
    }
    const QSharedPointer<GWSGraphNode> from_node = this->findNode( from );
    const QSharedPointer<GWSGraphNode> to_node = this->findNode( to );
    if( !from_node || !to_node ){
        return 0;
    }
    foreach(QSharedPointer<GWSGraphEdge> edge , this->edges){
        if( edge->getFrom() == from_node && edge->getTo() == to_node ){
            return edge;
        }
    }*/
    return 0;
}

QList<QSharedPointer<GWSGraphEdge>> GWSGraph::getEdges() const{
    return this->edges;
}

/*QList<QSharedPointer<GWSGraphEdge>> GWSGraph::getEdgesInEnvelope(GWSEnvelope env) const{
    QList<QSharedPointer<GWSGraphEdge>> edges;
    for(unsigned int i = 0; i < this->dirEdges.size(); i++){
        geos::planargraph::DirectedEdge* e  = dynamic_cast<geos::planargraph::DirectedEdge*>( this->dirEdges.at(i) );
        geos::geom::Coordinate c2 = e->getToNode()->getCoordinate();
        geos::geom::Coordinate c1 = e->getFromNode()->getCoordinate();
        if( env.contains( GeoCoordinates( c1.x , c1.y , c1.z ) ) && env.contains( GeoCoordinates( c2.x , c2.y , c2.z ) ) ){
            edges.append( dynamic_cast<QSharedPointer<GWSGraphEdge>>( this->dirEdges.at(i) )  );
        }
    }
    return edges;
}*/

/*QList<const QSharedPointer<GWSGraphNode>> GWSGraph::getNodes() const{
    QList<const QSharedPointer<GWSGraphNode>> nodes;
    nodes.append( this->nodes );
    return nodes;
}*/

QMap<QSharedPointer<GWSGraphEdge>,double> GWSGraph::getCostMap() const{
    QMap<QSharedPointer<GWSGraphEdge> , double> cost_map;
    foreach( QSharedPointer<GWSGraphEdge> edge , this->edges ){
        cost_map.insert( edge , edge->getCost() );
    }
    return cost_map;
}

/*QList<QSharedPointer<GWSGraphNode>> GWSGraph::findNodesOfDegree( int degree ) const{
    QList<QSharedPointer<GWSGraphNode>> nodes;
    foreach( QSharedPointer<GWSGraphNode> node , this->coordinates) {
        if( node->getDegree() == degree ){
            nodes.append( node );
        }
    }
    return nodes;
}*/

int GWSGraph::countNodes() const{
    return this->coordinates.size();
}

int GWSGraph::countEdges() const{
    return this->edges.size();
}

/*MultiLineString* GWSGraph::toMultiLineString() const{
    std::vector<Geometry*> *lines = new std::vector<Geometry*>();
    foreach( QSharedPointer<GWSGraphEdge> e , this->getEdges() ){
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
    foreach( QSharedPointer<GWSGraphEdge> edge , other->edges ){
            this->addEdge( edge );
    }
}

void GWSGraph::addEdge( QSharedPointer<GWSGraphEdge> edge){
    GWSCoordinate from_coor = edge->getFrom();
    GWSCoordinate to_coor = edge->getTo();
    if( !this->coordinates.contains( from_coor ) ){
        this->coordinates.append( from_coor );
    }
    if( this->coordinates.contains( to_coor ) ){
        this->coordinates.append( to_coor );
    }
    this->edges.append( edge );
    this->edges_index->upsert( edge , edge->getFrom() );
}

void GWSGraph::removeEdge(QSharedPointer<GWSGraphEdge> edge){
    this->edges.removeAll( edge );
}

/*void GWSGraph::addNode(QSharedPointer<GWSGraphNode> node){
    this->nodes_index->upsert( node , node->inner_coordinate );
    this->coordinates.append( node );
}

void GWSGraph::removeNode(QSharedPointer<GWSGraphNode> node){
    this->nodes_index->remove( node );
    this->coordinates.removeAll( node );
}*/

