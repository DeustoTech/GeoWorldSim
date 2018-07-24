#ifndef GWSGRAPHNODE_H
#define GWSGRAPHNODE_H

#include <QList>

#include "../util/geometry/Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSGraphEdge)

class GWSGraphNode
{
    friend class GWSGraphEdge;

public:
    GWSGraphNode();

    // GETTERS
    virtual const QList<GWSGraphEdge*> getDepartingEdges() const;
    virtual const QList<GWSGraphEdge*> getArrivingEdges() const;
    virtual const GWSGraphEdge* getDepartingEdgeTo( GWSGraphNode* to ) const;
    virtual const GWSGraphEdge* getArrivingEdgeFrom( GWSGraphNode* from ) const;
    virtual int getDegree() const;
    GWSCoordinate getCoordinate();

    // SETTERS

    // METHODS
    virtual double getCost() const; // To be implemented by each child
    bool equals(GWSGraphNode* other);

private:
    void connectEdge(GWSGraphEdge* edge);
    void disconnectEdge(GWSGraphEdge* edge);

    GWSCoordinate inner_coordinate;
    QList<GWSGraphEdge*> out_edges; // Departing edges
    QList<GWSGraphEdge*> in_edges; // Arriving edges
};

#endif // GWSGRAPHNODE_H
