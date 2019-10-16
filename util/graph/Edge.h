#ifndef EDGE_H
#define EDGE_H

#include <QString>


namespace geoworldsim {
namespace graph {

struct Edge {

    friend class EdgeVisitor;

public:

    // CONSTRUCTORS
    Edge() : cost(1) , type("None") , from_hash("None") , to_hash("None"){}
    Edge(double cost , QString type , QString from_hash , QString to_hash ) : cost(cost) , type(type) , from_hash(from_hash) , to_hash(to_hash){}
    Edge(const Edge &other) : Edge(other.cost , other.type , other.from_hash , other.to_hash){}
    ~Edge(){}

    // GETTERS
    virtual bool isValid() const;
    double getCost( double accumulated_cost = 0 ) const;
    QString getType() const;

    bool operator == (const Edge&) const;
    bool operator != (const Edge&) const;
    bool operator < (const Edge&) const;
    Edge& operator= (const Edge&);

    // Returns an unique ID
    virtual QString getFromNodeUID() const { return this->from_hash; }
    virtual QString getToNodeUID() const { return this->to_hash; }

protected:

    // PROPERTIES
    double cost = 1; // User for length, time, price, etc
    QString type = "None"; // Used to determine what type of relation
    const QString from_hash;
    const QString to_hash;

};

}
}

#endif // EDGE_H
