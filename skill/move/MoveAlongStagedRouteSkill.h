#ifndef MOVEALONGSTAGEDROUTESKILL_H
#define MOVEALONGSTAGEDROUTESKILL_H

#include "../../skill/move/MoveThroughRouteSkill.h"
#include "../../util/routing/TSPRouting.h"

class MoveAlongStagedRouteSkill : public MoveThroughRouteSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveAlongStagedRouteSkill();

    //- PROPERTIES
    static QString EDGES_CLASS_PROP;
    //static QList< GWSCoordinate> ROUTE_STAGES_PROP;

    // GETTERS
    //GWSCoordinate getRouteDestination() const;

    // METHODS
    //virtual void generateTSPGraph();
    virtual void step(GWSTimeUnit movement_duration);

protected:
    //const GWSGraph* graph = Q_NULLPTR;
    //GSSTSPRouting* tsp_route;

};

#endif // MOVEALONGSTAGEDROUTESKILL_H
