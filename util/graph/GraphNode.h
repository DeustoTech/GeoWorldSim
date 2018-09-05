#ifndef GWSGRAPHNODE_H
#define GWSGRAPHNODE_H

#include <QList>

#include "../../object/Object.h"
#include "../../util/geometry/Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSGraph)
QT_FORWARD_DECLARE_CLASS(GWSGraphEdge)

class GWSGraphNode: public GWSObject
{
    Q_OBJECT
    friend class GWSGraphEdge;
    friend class GWSGraph;

public:
    Q_INVOKABLE explicit GWSGraphNode();
    ~GWSGraphNode();

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // EXPORTERS
    virtual QJsonObject serialize() const;

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

    GWSCoordinate inner_coordinate;
    QList< QSharedPointer<GWSGraphEdge> > out_edges; // Departing edges
    QList< QSharedPointer<GWSGraphEdge> > in_edges; // Arriving edges
};

#endif // GWSGRAPHNODE_H
