#ifndef MOVETHROUGHTRANSPORTLINESKILL_H
#define MOVETHROUGHTRANSPORTLINESKILL_H

#include "skills/move/MoveSkill.h"

#include "agents/transport_networks/TransportLine.h"

QT_FORWARD_DECLARE_CLASS(Vehicle)

class MoveThroughTransportLineSkill : public MoveSkill
{
    Q_OBJECT

public:
    MoveThroughTransportLineSkill(Vehicle* skilled_agent);
    ~MoveThroughTransportLineSkill();

    // METHODS
    virtual void moveTo( TransportLine* transport_line );

private:
    TransportLine* current_edge = Q_NULLPTR;
};

#endif // MOVETHROUGHTRANSPORTLINESKILL_H
