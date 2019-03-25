#ifndef GWSCOORDINATE_H
#define GWSCOORDINATE_H

#include <QObject>
#include <QString>
#include <QtMath>

#include "../units/Units.h"

struct GWSCoordinate {

    // PROPERTIES
    double x = std::nan("0");
    double y = std::nan("0");
    double z = std::nan("0");

    // CONSTRUCTORS
    GWSCoordinate() : x( std::nan("0") ) , y( std::nan("0") ) , z( std::nan("0") ){}
    GWSCoordinate(double x, double y, double z = 0.0) : x(x) , y(y) , z(z){
        if(this->z != this->z ){ this->z = 0; }
    }
    GWSCoordinate(const GWSCoordinate &other) : GWSCoordinate(other.x , other.y, other.z){}
    ~GWSCoordinate(){}

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    double getX() const;
    double getY() const;
    double getZ() const;
    GWSLengthUnit getDistance( GWSCoordinate other ) const;
    double getLatLngDistance( GWSCoordinate other ) const;
    bool isInBounds( double minX , double maxX , double minY , double maxY ) const;
    QString toString() const;

    // OPERATORS
    bool operator == (const GWSCoordinate&) const;
    bool operator != (const GWSCoordinate&) const;
    bool operator < (const GWSCoordinate&) const;
    GWSCoordinate& operator= (const GWSCoordinate&);
    GWSCoordinate& operator+ (const GWSCoordinate&);
    GWSCoordinate& operator- (const GWSCoordinate&);

};


#endif // GWSCOORDINATE_H
