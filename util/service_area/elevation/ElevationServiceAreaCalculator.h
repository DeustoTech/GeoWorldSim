#ifndef ELEVATIONSERVICEAREACALCULATOR_H
#define ELEVATIONSERVICEAREACALCULATOR_H

#include <QObject>

#include "utils/geometry/GSSGeometryController.h"
#include "utils/service_area/ServiceAreaCalculator.h"

class ElevationServiceAreaCalculator : public ServiceAreaCalculator{

    Q_OBJECT

public:
    ElevationServiceAreaCalculator();

    // GETTERS
    static GSSGeometry* getNeighbouringHigherCells(GSSCoordinate coord, GSSGrid grid, GSSGeometryController* controller );

private:
    static GSSGeometry* getNeighbouringHigherCellsRecursive(GSSCoordinate coord, GSSGrid grid, GSSGeometryController* controller, QList<GSSEnvelope> surrounding_cells, QList<GSSEnvelope> *visited_cells );

};

#endif // ELEVATIONSERVICEAREACALCULATOR_H
