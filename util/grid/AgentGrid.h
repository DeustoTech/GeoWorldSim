#ifndef GWSAGENTGRID_H
#define GWSAGENTGRID_H

#include "Grid.h"

class GWSAgentGrid : public GWSGrid
{

public:
    GWSAgentGrid( GWSAgent* agent );

    // GETTERS
    QList<GWSAgent*> getGridCellValue( unsigned int grid_x , unsigned int grid_y );

    // SETTERS
    //virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);
    //virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);

    // METHODS
    void enter( GWSAgent* agent );
    void exit( GWSAgent* agent );

private:
    QList<GWSAgent*> agents_inside;
};

#endif // GWSAGENTGRID_H
