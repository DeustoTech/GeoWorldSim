#include "GraphUtils.h"

#include <QDebug>

GWSGraphUtils* GWSGraphUtils::globalInstance(){
    static GWSGraphUtils instance;
    return &instance;
}

GWSGraphUtils::GWSGraphUtils(QObject *parent) : QObject(parent){
}

GWSGraphUtils::~GWSGraphUtils(){
}

/**********************************************************************
 METHODS
**********************************************************************/

/**
 * WARNING Caller gets ownership ONLY of created simplified version of GSSGraphEdges
 * @brief GSSGraphUtils::createSimplification
 * @param cost_map
 * @return
 */
QMap<GWSGraphEdge* , QList<GWSGraphEdge*> > GWSGraphUtils::createSimplification( const GWSGraph* graph ){

    QMap<GWSGraphEdge* , QList<GWSGraphEdge*> > simplified_to_real; // QMAP< SIMPLIFIED_EDGE , QLIST< ORIGINAL EDGES > >
    QMap<GWSGraphNode* , GWSGraphNode*> old_to_new_nodes;

    QList<GWSGraphEdge*> used_edges;
    foreach( GWSGraphEdge* edge , graph->getEdges() ){

        if( used_edges.contains(edge) ){
            continue;
        }

        // Find an edge starting in an intersection/dead end
        if( edge->getFromNode()->getDegree() != 2 ){

            // If degree == 1; starts in a dead end
            // If degree > 2; starts in an intersection

            // Equivalence list
            QList<GWSGraphEdge*> new_edge_to_old_edges;

            // Iterate edge until arriving at degree != 2 ToNode
            GWSGraphNode* from = edge->getFromNode();
            GWSGraphEdge* current_edge = edge;

            GWSGraphNode* to = current_edge->getToNode();
            GWSLengthUnit acc_length = current_edge->getLength();
            new_edge_to_old_edges.append( current_edge );

            // While degree == 2; Not dead end nor intersection node
            while( current_edge->getToNode()->getDegree() == 2 ){

                // Get next edge, that doesnt return back to the start point
                GWSGraphEdge* next_edge = 0;
                foreach( GWSGraphEdge* e , current_edge->getToNode()->getDepartingEdges() ){

                    // Should only be 1 (if one way direction) or 2 (if double direction)
                    // Choose the one that doesnt return back to our start point
                    if( !e->getToNode()->equals( current_edge->getFromNode() ) ){
                        next_edge = e;
                    }
                }

                // If there is no next_edge, dead end. Finish loop
                if( !next_edge ){ break; }

                current_edge = next_edge;
                to = current_edge->getToNode();
                acc_length = acc_length + current_edge->getLength();
                new_edge_to_old_edges.append( current_edge );
            }

            // Create new nodes, not to connect the new edges to real nodes
            GWSGraphNode* new_from = old_to_new_nodes.value( from , 0 );
            if( !new_from ){
                new_from = new GWSGraphNode( from->getCoordinate() );
            }
            GWSGraphNode* new_to = old_to_new_nodes.value( to , 0 );
            if( !new_to ){
                new_to = new GWSGraphNode( to->getCoordinate() );
            }

            // If from and to are the same, do not create edge
            if( !new_from->equals( new_to ) ){

                GWSGraphEdge* new_edge = new GWSGraphEdge( new_from , new_to );
                new_edge->length = acc_length;
                simplified_to_real.insert( new_edge , new_edge_to_old_edges );
            }

            used_edges.append( new_edge_to_old_edges );
        }
    }

    qInfo() << QString("Simplified %1 into %2 edges").arg( used_edges.size() ).arg( simplified_to_real.size() );

    return simplified_to_real;
}

GWSGraphNode* GWSGraphUtils::findNearestFromQuadtree(GWSCoordinate coor , const GWSQuadtree *nodes_index){
    GWSGraphNode* found = 0;
    QList<void *> objs = nodes_index->getElements( coor );

    if( objs.isEmpty() ){
        return found;
    }

    found = ((GWSGraphNode*) objs.at( 0 ) );
    GeoCoordinates found_coor = found->getCoordinate();
    double found_distance = coor.distance( found_coor );
    for(int i = 0 ; i < objs.size() ; i++){
        GWSGraphNode* g = ((GWSGraphNode*) objs.at(i));
        if( g ){

            try {
                double d = coor.distance( g->getCoordinate() );
                if( d <= found_distance ){
                    found = g;
                    found_distance = d;
                }
            } catch(...){}
        }
    }
    return found;
}
