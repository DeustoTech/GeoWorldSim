#ifndef GWSOLDNETWORKEDGE_H
#define GWSOLDNETWORKEDGE_H

#include "../../util/graph/OldEdge.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/units/Units.h"

class GWSOldNetworkEdge : public GWSOldEdge
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSOldNetworkEdge();
    ~GWSOldNetworkEdge();

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
    virtual QString getFromNodeId() const;
    virtual QString getToNodeId() const;
    virtual GWSCoordinate getFromCoordinate() const;
    virtual GWSCoordinate getToCoordinate() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down

    // SETTERS
    void setLength( GWSLengthUnit length );

};

#endif // GWSOLDNETWORKEDGE_H
