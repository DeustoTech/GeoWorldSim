#ifndef NETWORKEDGE_H
#define NETWORKEDGE_H

#include <QJsonObject>

#include "../../util/graph/Edge.h"
#include "../../util/geometry/Coordinate.h"

namespace geoworldsim {
namespace graph {

struct NetworkEdge : Edge {

public:

    // JSON
    static QString EDGE_FROM_X_PROP;
    static QString EDGE_FROM_Y_PROP;
    static QString EDGE_FROM_Z_PROP;
    static QString EDGE_TO_X_PROP;
    static QString EDGE_TO_Y_PROP;
    static QString EDGE_TO_Z_PROP;

    // CONSTRUCTORS
    NetworkEdge() : Edge() {}
    NetworkEdge( QJsonObject json ) : NetworkEdge( geometry::Coordinate( json.value( EDGE_FROM_X_PROP ).toDouble() , json.value( EDGE_FROM_Y_PROP ).toDouble() , json.value( EDGE_FROM_Z_PROP ).toDouble() ) , geometry::Coordinate( json.value( EDGE_TO_X_PROP ).toDouble() , json.value( EDGE_TO_Y_PROP ).toDouble() , json.value( EDGE_TO_Z_PROP ).toDouble() ) ) {}
    NetworkEdge( const geometry::Coordinate &from , const geometry::Coordinate &to ) : Edge( from.getDistance(to).number() , "Network" , from.toString() , to.toString() ) , from(from) , to(to) {}
    NetworkEdge(const NetworkEdge &other) : NetworkEdge(other.from , other.to){}
    ~NetworkEdge(){}

    // GETTERS
    virtual bool isValid() const;
    virtual geometry::Coordinate getFromCoordinate() const;
    virtual geometry::Coordinate getToCoordinate() const;
    virtual unit::LengthUnit getLength() const;
    virtual double getGradient() const; // Positive for climbing up and negative for going down

    // SETTERS
    void setLength( unit::LengthUnit length );

protected:

    // PROPERTIES
    geometry::Coordinate from;
    geometry::Coordinate to;

};


}
}

#endif // NETWORKEDGE_H
