#ifndef GWSEDGE_H
#define GWSEDGE_H

#include <QString>

struct GWSEdge {

    // PROPERTIES
    double cost = 1; // User for length, time, price, etc
    QString type = "None"; // Used to determine what type of relation
    const QString from_hash;
    const QString to_hash;

    // CONSTRUCTORS
    GWSEdge() : cost(1) , type("None") , from_hash("None") , to_hash("None"){}
    GWSEdge(double cost , QString type , QString from_hash , QString to_hash ) : cost(cost) , type(type) , from_hash(from_hash) , to_hash(to_hash){}
    GWSEdge(const GWSEdge &other) : GWSEdge(other.cost , other.type , other.from_hash , other.to_hash){}
    ~GWSEdge(){}

    // GETTERS
    virtual bool isValid() const;
    double getCost( double accumulated_cost = 0 ) const;
    QString getType() const;

    bool operator == (const GWSEdge&) const;
    bool operator != (const GWSEdge&) const;
    bool operator < (const GWSEdge&) const;
    GWSEdge& operator= (const GWSEdge&);

    // Returns an unique ID
    QString getFromNodeUID() const { return this->from_hash; }
    QString getToNodeUID() const { return this->to_hash; }

};

#endif // GWSEDGE_H
