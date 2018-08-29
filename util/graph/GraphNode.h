#ifndef GWSGRAPHNODE_H
#define GWSGRAPHNODE_H

#include <QList>

#include "../../util/geometry/Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSAgent)
QT_FORWARD_DECLARE_CLASS(GWSGraphEdge)

class GWSGraphNode
{
    friend class GWSGraphEdge;

public:
    GWSGraphNode( GWSAgent* agent );

    // GETTERS
    virtual const QList< QSharedPointer<GWSGraphEdge> > getDepartingEdges() const;
    virtual const QList< QSharedPointer<GWSGraphEdge> > getArrivingEdges() const;
    virtual const QSharedPointer<GWSGraphEdge> getDepartingEdgeTo( QSharedPointer<GWSGraphNode> to ) const;
    virtual const QSharedPointer<GWSGraphEdge> getArrivingEdgeFrom( QSharedPointer<GWSGraphNode> from ) const;
    virtual int getDegree() const;
    GWSCoordinate getCoordinate();

    // SETTERS

    // METHODS
    virtual double getCost() const; // To be implemented by each child
    bool equals(QSharedPointer<GWSGraphNode> other);

private:
    void connectEdge(QSharedPointer<GWSGraphEdge> edge);
    void disconnectEdge(QSharedPointer<GWSGraphEdge> edge);

    // REF AGENT
    GWSAgent* agent;

    GWSCoordinate inner_coordinate;
    QList< QSharedPointer<GWSGraphEdge> > out_edges; // Departing edges
    QList< QSharedPointer<GWSGraphEdge> > in_edges; // Arriving edges
};

#endif // GWSGRAPHNODE_H
