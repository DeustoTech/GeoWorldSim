#ifndef INTERPOLATIONGRID_H
#define INTERPOLATIONGRID_H

#include "utils/grid/Grid.h"

class InterpolationGrid : public GWSGrid
{
    Q_OBJECT

public:
    explicit InterpolationGrid(double left , double right , double top , double bottom , unsigned int x_size = 100 , unsigned int y_size = 100 , double min_value = NAN, double max_value = NAN , GWSObject *parent = Q_NULLPTR);
    InterpolationGrid(const InterpolationGrid&);
    InterpolationGrid(const GWSGrid&);
    InterpolationGrid();

    // GETTERS
    virtual double getValue(GWSCoordinate coor) const;

};

#endif // INTERPOLATIONGRID_H
