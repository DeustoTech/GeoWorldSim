#ifndef GWSGEOGRID_H
#define GWSGEOGRID_H

#include "Grid.h"

#include "../geometry/Point.h"
#include "../geometry/Polygon.h"

class GWSGeoGrid : public GWSGrid
{
    Q_OBJECT

public:
    GWSGeoGrid();

    // PROPERTIES
    static QString LEFT_BOUNDS_PROP;
    static QString RIGHT_BOUNDS_PROP;
    static QString TOP_BOUNDS_PROP;
    static QString BOTTOM_BOUNDS_PROP;

    // GETTERS
    GWSPolygon* getGridBounds() const;
    virtual double getCellValue( const GWSPoint* point ) const;
    GWSPolygon* getCellEnvelope( unsigned int grid_x , unsigned int grid_y ) const;

};

Q_DECLARE_METATYPE(GWSGeoGrid)

#endif // GWSGEOGRID_H
