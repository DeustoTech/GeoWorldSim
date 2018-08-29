#ifndef GWSAGENTGRID_H
#define GWSAGENTGRID_H

#include "Grid.h"

class GWSAgentGrid : public GWSGrid
{

public:
    GWSAgentGrid( QSharedPointer<GWSAgent> agent );

    // GETTERS
    QList< QSharedPointer<GWSAgent> > getGridCellValue( unsigned int grid_x , unsigned int grid_y );

    // SETTERS
    //virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);
    //virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);

    // METHODS
    void enter( QSharedPointer<GWSAgent> agent );
    void exit( QSharedPointer<GWSAgent> agent );

private:
    QList< QSharedPointer<GWSAgent> > agents_inside;
};

#endif // GWSAGENTGRID_H
