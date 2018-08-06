#ifndef GWSAGENTGRID_H
#define GWSAGENTGRID_H

#include "Grid.h"

class GWSAgentGrid : public GWSGrid
{

public:
    GWSAgentGrid( GWSAgent* agent );


    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QList<GWSAgent*> getGridCellValue( unsigned int grid_x , unsigned int grid_y );

    // SETTERS
    virtual void setGridSize( unsigned int x_size = 100, unsigned int y_size = 100);
    virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);
    virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , GWSAgent* v);

private:
    QVector< QVector < QList<GWSAgent*> > > values;
};

#endif // GWSAGENTGRID_H
