#ifndef GWSNEWEDGE_H
#define GWSNEWEDGE_H

#include <QString>

struct GWSNewEdge {

    // PROPERTIES
    double cost = 1; // User for length, time, price, etc
    QString type = "None"; // Used to determine what type of relation

    // CONSTRUCTORS
    GWSNewEdge() : cost(1) , type("None"){}
    GWSNewEdge(double cost, QString type) : cost(cost) , type(type){}
    GWSNewEdge(const GWSNewEdge &other) : GWSNewEdge(other.cost , other.type){}
    ~GWSNewEdge(){}

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    double getCost( double accumulated_cost = 0 ) const;
    QString getType() const;

    bool operator == (const GWSNewEdge&) const;
    bool operator != (const GWSNewEdge&) const;
    bool operator < (const GWSNewEdge&) const;
    GWSNewEdge& operator= (const GWSNewEdge&);

    // Returns an unique ID
    virtual QString getFromNodeUID() const = 0;
    virtual QString getToNodeUID() const = 0;

};

#endif // GWSNEWEDGE_H
