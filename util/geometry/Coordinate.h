#ifndef COORDINATE_H
#define COORDINATE_H

#include <QObject>
#include <QString>
#include <QtMath>

#include "../units/Units.h"

namespace geoworldsim {
namespace geometry {


struct Coordinate {

    // PROPERTIES
    double x = std::nan("0");
    double y = std::nan("0");
    double z = std::nan("0");

    // CONSTRUCTORS
    Coordinate() : x( std::nan("0") ) , y( std::nan("0") ) , z( std::nan("0") ){}
    Coordinate(double x , double y , double z = 0.0) : x(x) , y(y) , z(z){
        if(this->z != this->z ){ this->z = 0; }
    }
    Coordinate( const Coordinate &other ) : Coordinate( other.x , other.y , other.z){}
    ~Coordinate(){}

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    double getX() const;
    double getY() const;
    double getZ() const;
    unit::LengthUnit getDistance( const Coordinate &other ) const;
    double getLatLngDistance( const Coordinate &other ) const;
    bool isInBounds( double minX , double maxX , double minY , double maxY ) const;
    QString toString() const;

    // OPERATORS
    bool operator == (const Coordinate&) const;
    bool operator != (const Coordinate&) const;
    bool operator < (const Coordinate&) const;
    Coordinate& operator= (const Coordinate&);
    Coordinate& operator+ (const Coordinate&);
    Coordinate& operator- (const Coordinate&);

};

}
}

#endif // COORDINATE_H
