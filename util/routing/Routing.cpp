#include "Routing.h"

#include <QPainter>
#include <QStringList>
#include "../../util/geometry/Quadtree.h"

/**
 * @brief RoutingGraph::RoutingGraph
 * @param graph
 */
GWSRouting::GWSRouting( QList< QSharedPointer<GWSGraphEdge> > edges ) : GWSObject(){

    // Create graph and edge weights map
    this->nodes_index = new GWSQuadtree();
    this->routing_graph = new ListDigraph();

    // Temporary list to fasten comparisons
    QList< GWSCoordinate > fast_compare_node_coors;

    // Create nodes and edges
    foreach( QSharedPointer<GWSGraphEdge> edge, edges ){

        try {

            // Create or retrieve edge start node
            GWSCoordinate from_coor = edge->getFrom();
            ListDigraph::Node s; // Start node

            if( !fast_compare_node_coors.contains( from_coor ) ){
                s = this->routing_graph->addNode();
                //this->nodes_index->upsert( from_node.dynamicCast<GWSAgent>() , from_node->getCoordinate() );
                this->coors_to_node.insert( from_coor , s );
                fast_compare_node_coors.append( from_coor );
            } else {
                s = this->coors_to_node.value( from_coor );
            }

            // Create or retrieve edge end node
            GWSCoordinate to_coor = edge->getTo();
            ListDigraph::Node e;

            if( !fast_compare_node_coors.contains( to_coor ) ){
                e = this->routing_graph->addNode();
                //this->nodes_index->upsert( to_node->getCoordinate() , to_node );
                this->coors_to_node.insert( to_coor , e );
                fast_compare_node_coors.append( to_coor );
            } else {
                e = this->coors_to_node.value( to_coor );
            }

            // Create arc and link it to edge
            ListDigraph::Arc arc = this->routing_graph->addArc(s , e);
            this->arc_to_edges.insert( arc , edge );

        } catch(...){}
    }
}

GWSRouting::~GWSRouting(){
    // Delete created stuff
    delete this->nodes_index;
    delete this->routing_graph;
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/*QImage GWSRouting::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height) const{

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QColor(0,0,0) );

    foreach( GWSGraphEdge* edge , this->arc_to_edges.values() ){

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

/*QSharedPointer<GWSGraphNode> GWSRouting::findNearestNode( const GWSCoordinate coor ){
    return this->nodes_index->getNearestElement( coor ).dynamicCast<GWSGraphNode>();
}

QSharedPointer<GWSGraphNode> GWSRouting::getNodeFromNode(const ListDigraph::Node node){
    return this->coors_to_node.value( node );
}

QSharedPointer<GWSGraphEdge> GWSRouting::getEdgeFromArc(const ListDigraph::Arc arc){
    return this->arc_to_edges.value( arc );
}*/
