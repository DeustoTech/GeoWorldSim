#ifndef GWSEDGE_H
#define GWSEDGE_H

#include <QString>

struct GWSEdge {

    // PROPERTIES
    double cost = 1; // User for length, time, price, etc
    QString type = "None"; // Used to determine what type of relation

    // CONSTRUCTORS
    GWSEdge() : cost(1) , type("None"){}
    GWSEdge(double cost, QString type) : cost(cost) , type(type){}
    GWSEdge(const GWSEdge &other) : GWSEdge(other.cost , other.type){}
    ~GWSEdge(){}

    // GETTERS
    QString getUID() const;
    bool isValid() const;
    double getCost( double accumulated_cost = 0 ) const;
    QString getType() const;

    bool operator == (const GWSEdge&) const;
    bool operator != (const GWSEdge&) const;
    bool operator < (const GWSEdge&) const;
    GWSEdge& operator= (const GWSEdge&);

    // Returns an unique ID
    virtual QString getFromNodeUID() const = 0;
    virtual QString getToNodeUID() const = 0;

};

#endif // GWSEDGE_H
