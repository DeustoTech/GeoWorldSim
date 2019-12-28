#ifndef GWSOLDEDGE_H
#define GWSOLDEDGE_H

#include "../../object/Object.h"

class GWSOldEdge : public GWSObject
{
    Q_OBJECT

public:

    GWSOldEdge(); // Cannot be invokable because it has abstract methods

    // GETTERS
    virtual double getEdgeCost( double accumulated_cost = 0 ) const;
    virtual QString getEdgeType() const;
    bool equals( const QSharedPointer<GWSOldEdge> other ) const;
    bool equalsReversed( const QSharedPointer<GWSOldEdge> other ) const;

    // Returns an unique ID
    virtual QString getFromNodeId() const = 0;
    virtual QString getToNodeId() const = 0;

protected:
    double cost = 1; // User for length, time, price, etc
    QString type = "None"; // Used to determine what type of relation

};

#endif // GWSOLDEDGE_H
