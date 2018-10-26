#ifndef MOVETHROUGHROUTESKILL_H
#define MOVETHROUGHROUTESKILL_H

#include <QObject>
#include "../../util/graph/GraphEdge.h"
#include "../../skill/move/MoveSkill.h"

class MoveThroughRouteSkill : public MoveSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MoveThroughRouteSkill();
    ~MoveThroughRouteSkill();

    //- PROPERTIES
    static QString NETWORK_CLASS_PROP;
    static QString ROUTE_DESTINATION_X_PROP;
    static QString ROUTE_DESTINATION_Y_PROP;

    // GETTERS
    GWSCoordinate getRouteDestination() const;

    // METHODS
    virtual void move(GWSTimeUnit movement_duration);

protected:

    QList< QSharedPointer<GWSGraphEdge> > pending_route;
    QList<GWSCoordinate> pending_edge_coordinates;

};

#endif // MOVETHROUGHROUTESKILL_H
