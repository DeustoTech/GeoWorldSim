#include "Routing.h"

#include <QPainter>
#include "utils/geometry/Envelope.h"
#include "utils/conversors/image_coordinates/ImageCoordinatesConversor.h"

#include "utils/graph/GraphUtils.h"

/**
 * @brief RoutingGraph::RoutingGraph
 * @param graph
 */
GWSRouting::GWSRouting( QList<GWSGraphEdge*> edges ) : GWSObject(0){

    // Create graph and edge weights map
    this->nodes_index = new GWSQuadtree();
    this->routing_graph = new ListDigraph();

    // Temporary list to fasten comparisons
    QList<GWSGraphNode*> fast_compare_nodes;

    // Create nodes and edges
    foreach( GWSGraphEdge* edge, edges ){

        try {

            // Create or retrieve edge start node
            GWSGraphNode* from_node = edge->getFromNode();
            ListDigraph::Node s;

            if( !fast_compare_nodes.contains( from_node ) ){
                s = this->routing_graph->addNode();
                this->nodes_index->insert( from_node->getCoordinate() , from_node );
                this->node_to_nodes.insert( s , from_node );
                fast_compare_nodes.append( from_node );
            } else {
                s = this->node_to_nodes.key( from_node );
            }

            // Create or retrieve edge end node
            GWSGraphNode* to_node = edge->getToNode();
            ListDigraph::Node e;

            if( !fast_compare_nodes.contains( to_node ) ){
                e = this->routing_graph->addNode();
                this->nodes_index->insert( to_node->getCoordinate() , to_node );
                this->node_to_nodes.insert( e , to_node );
                fast_compare_nodes.append( to_node );
            } else {
                e = this->node_to_nodes.key( to_node );
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

QImage GWSRouting::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height) const{

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
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSGraphNode* GWSRouting::findNearestNode( const GWSCoordinate coor ){
    return GWSGraphUtils::globalInstance()->findNearestFromQuadtree( coor , this->nodes_index );
}

GWSGraphNode* GWSRouting::getNodeFromNode(const ListDigraph::Node node){
    return this->node_to_nodes.value( node );
}

GWSGraphEdge* GWSRouting::getEdgeFromArc(const ListDigraph::Arc arc){
    return this->arc_to_edges.value( arc );
}
