#ifndef GWSNETWORKEDGE_H
#define GWSNETWORKEDGE_H

#include "../../util/graph/Edge.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/units/Units.h"

class GWSNetworkEdge : public GWSEdge
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSNetworkEdge();
    ~GWSNetworkEdge();

    // PROPERTIES
    static QString EDGE_FROM_X_PROP;
    static QString EDGE_FROM_Y_PROP;
    static QString EDGE_FROM_Z_PROP;
    static QString EDGE_TO_X_PROP;
    static QString EDGE_TO_Y_PROP;
    static QString EDGE_TO_Z_PROP;

    // IMPORTERS
    virtual void deserialize( QJsonObject json , QSharedPointer<GWSObject> parent = QSharedPointer<GWSObject>() );

    // GETTERS
    virtual GWSCoordinate getFrom() const;
    virtual GWSCoordinate getTo() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down
    virtual bool equals( const QSharedPointer<GWSNetworkEdge> other) const;
    virtual bool equalsReversed( const QSharedPointer<GWSNetworkEdge> other ) const;

    // SETTERS
    void setLength(GWSLengthUnit length);

    // METHODS
    virtual double getCost( double accumulated_cost = 0 ) const; // To be implemented by each child

private:
    GWSLengthUnit length = 0;
};

#endif // GWSNETWORKEDGE_H
