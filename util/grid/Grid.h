#ifndef GWSGRID_H
#define GWSGRID_H

#include <QVector>
#include <QJsonArray>

struct GWSGrid
{
    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

    // PROPERTIES
    QJsonArray grid;
    double max_value = -1;
    double min_value = -1;

    // CONSTRUCTORS
    GWSGrid(){}
    GWSGrid( QJsonArray grid );
    GWSGrid(const GWSGrid &other) : GWSGrid(other.grid){}
    ~GWSGrid();

    // PROPERTIES
    static QString GRID_MAX_VALUE_PROP;
    static QString GRID_MIN_VALUE_PROP;
    static QString GRID_VALUES_PROP;
    static QString GRID_X_SIZE_PROP;
    static QString GRID_Y_SIZE_PROP;

    // GETTERS
    virtual bool isGridEmpty() const;
    int getGridXSize() const;
    int getGridYSize() const;
    double getGridMaxValue() const;
    double getGridMinValue() const;
    virtual double getGridCellValue( int grid_x , int grid_y ) const;
    //virtual void getGridCellValue( unsigned int grid_x , unsigned int grid_y ) = 0;

    // SETTERS
    void setGridMaxValue( double max );
    void setGridMinValue( double min );
    //void setCellValue( GWSCoordinate* coor , double v );
    //virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void setGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v);

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

};


#endif // GWSGRID_H
