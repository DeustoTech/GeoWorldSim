#ifndef GWSGRID_H
#define GWSGRID_H

#include <QVector>
#include <QJsonObject>
#include <QImage>

#include "util/geometry/Coordinate.h"
#include "util/geometry/Envelope.h"

class GWSGrid
{

    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

public:
    explicit GWSGrid(double left , double right , double top , double bottom , unsigned int x_size = 100 , unsigned int y_size = 100 , double min_value = NAN, double max_value = NAN );
    GWSGrid(const GWSGrid&);
    GWSGrid();
    ~GWSGrid();

    // EXPORTERS
    QImage toImage( const GWSEnvelope image_bounds , int image_width = 1024, int image_height = 1024 ) const;

    // GETTERS
    bool isEmpty() const;
    const GWSEnvelope getBounds() const;
    unsigned int getXSize() const;
    unsigned int getYSize() const;
    double getMaxValue() const;
    double getMinValue() const;
    virtual double getValue( GWSCoordinate coor ) const;
    const GWSEnvelope getCellEnvelope( unsigned int grid_x , unsigned int grid_y ) const;

    // SETTERS
    void setBounds( double left , double right , double top , double bottom );
    void setBounds( GWSEnvelope bounds );
    void setSize( unsigned int x_size = 100 , unsigned int y_size = 100 );
    void setValue( GWSCoordinate coor , double v );

    // OPERATORS
    GWSGrid& operator=(const GWSGrid&);

    // METHODS
    GWSGrid getSubGrid( GWSEnvelope bounds );
    QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

protected:
    // GETTERS
    double getValue( unsigned int grid_x , unsigned int grid_y ) const;
    // SETTERS
    void setValue( unsigned int grid_x , unsigned int grid_y , double v);

private:
    GWSEnvelope bounds;
    QVector< QVector<double> > values;
    double min_value;
    double max_value;
};

#endif // GWSGRID_H
