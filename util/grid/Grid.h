#ifndef GWSGRID_H
#define GWSGRID_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>
#include <QJsonArray>
#include "../../util/geometry/Geometry.h"

class GWSGrid : public QObject {

    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

public:
    GWSGrid( GWSGeometry bounds , unsigned int x_size , unsigned int y_size );
    ~GWSGrid();

    // GETTERS
    virtual bool isGridEmpty() const;
    GWSGeometry getBounds() const;
    QJsonObject getGeoJSON() const;
    unsigned int getXSize() const;
    unsigned int getYSize() const;
    double getLon( double x ) const;
    double getLat( double y ) const;
    virtual QJsonValue getValue( GWSCoordinate coor ) const;
    virtual QJsonValue getValue( GWSGeometry geom ) const;
    //virtual void getGridCellValue( unsigned int grid_x , unsigned int grid_y ) = 0;

    // SETTERS
    void addValue( GWSCoordinate coor , QJsonValue value );
    void addValue( GWSGeometry geom , QJsonValue value );
    //virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void setGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v);

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

protected:

    mutable QReadWriteLock mutex;

    GWSGeometry grid_bounds;
    QMap< unsigned int , QMap< unsigned int , QJsonValue >* >* grid;
    int x_size = -1;
    int y_size = -1;

    // BOUNDS
    double min_x = 180;
    double max_x = -180;
    double min_y = 180;
    double max_y = -180;

};


#endif // GWSGRID_H
