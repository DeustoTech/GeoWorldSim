#ifndef GWSNETWORKEDGE_H
#define GWSNETWORKEDGE_H

#include <QJsonObject>

#include "../../util/graph/Edge.h"
#include "../../util/geometry/Coordinate.h"

struct GWSNewNetworkEdge : GWSEdge {

    // PROPERTIES
    GWSCoordinate from;
    GWSCoordinate to;

    // JSON
    static QString EDGE_FROM_X_PROP;
    static QString EDGE_FROM_Y_PROP;
    static QString EDGE_FROM_Z_PROP;
    static QString EDGE_TO_X_PROP;
    static QString EDGE_TO_Y_PROP;
    static QString EDGE_TO_Z_PROP;

    // CONSTRUCTORS
    GWSNewNetworkEdge() : GWSEdge() {}
    GWSNewNetworkEdge( QJsonObject json ) : GWSNewNetworkEdge( GWSCoordinate( json.value( EDGE_FROM_X_PROP ).toDouble() , json.value( EDGE_FROM_Y_PROP ).toDouble() , json.value( EDGE_FROM_Z_PROP ).toDouble() ) , GWSCoordinate( json.value( EDGE_TO_X_PROP ).toDouble() , json.value( EDGE_TO_Y_PROP ).toDouble() , json.value( EDGE_TO_Z_PROP ).toDouble() ) ) {}
    GWSNewNetworkEdge(GWSCoordinate from, GWSCoordinate to) : GWSEdge( from.getDistance(to).number() , "Network" ) , from(from) , to(to) {}
    GWSNewNetworkEdge(const GWSNewNetworkEdge &other) : GWSNewNetworkEdge(other.from , other.to){}
    ~GWSNewNetworkEdge(){}

    // GETTERS
    virtual QString getFromNodeUID() const;
    virtual QString getToNodeUID() const;
    virtual GWSCoordinate getFromCoordinate() const;
    virtual GWSCoordinate getToCoordinate() const;
    virtual GWSLengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down

    // SETTERS
    void setLength( GWSLengthUnit length );

};

#endif // GWSNETWORKEDGE_H
