#ifndef GWSGRAPHEDGE_H
#define GWSGRAPHEDGE_H

#include "../../util/units/Units.h"
#include "../../object/Object.h"

QT_FORWARD_DECLARE_CLASS(GWSGraphNode)

class GWSGraphEdge : public GWSObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSGraphEdge();
    ~GWSGraphEdge();

    // PROPERTIES
    static QString EDGE_FROM_X_PROP;
    static QString EDGE_FROM_Y_PROP;
    static QString EDGE_TO_X_PROP;
    static QString EDGE_TO_Y_PROP;

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    virtual QSharedPointer<GWSGraphNode> getFromNode() const;
    virtual QSharedPointer<GWSGraphNode> getToNode() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down
    virtual bool equals( const QSharedPointer<GWSGraphEdge> other) const;
    virtual bool equalsReversed( const QSharedPointer<GWSGraphEdge> other ) const;

    // SETTERS
    void setLength(GWSLengthUnit length);

    // METHODS
    virtual double getCost( double accumulated_cost = 0 ) const; // To be implemented by each child

private:

    QSharedPointer<GWSGraphNode> from;
    QSharedPointer<GWSGraphNode> to;
    GWSLengthUnit length = 0;
};

#endif // GWSGRAPHEDGE_H
