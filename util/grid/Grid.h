#ifndef GWSGRID_H
#define GWSGRID_H

#include <QVector>
#include <QJsonObject>
#include <QImage>

#include "../../object/Object.h"
#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Envelope.h"

class GWSGrid : public GWSObject
{
    Q_OBJECT

    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

public:
    Q_INVOKABLE explicit GWSGrid( QObject* parent = Q_NULLPTR );

    // PROPERTIES
    static QString MAX_VALUE_PROP;
    static QString MIN_VALUE_PROP;
    static QString BOUNDS_PROP;

    // EXPORTERS
    QImage toImage( const GWSEnvelope image_bounds , int image_width = 1024, int image_height = 1024 ) const;

    // GETTERS
    bool isEmpty() const;
    const GWSEnvelope* getBounds() const;
    unsigned int getXSize() const;
    unsigned int getYSize() const;
    double getMaxValue() const;
    double getMinValue() const;
    virtual double getCellValue( GWSCoordinate* coor ) const;
    double getCellValue( unsigned int grid_x , unsigned int grid_y ) const;
    //const GWSEnvelope getCellEnvelope( unsigned int grid_x , unsigned int grid_y ) const;

    // SETTERS
    void setBounds( GWSEnvelope* bounds );
    void setMaxValue( double max );
    void setMinValue( double min );
    void setSize( unsigned int x_size = 100 , unsigned int y_size = 100 );
    void setCellValue( GWSCoordinate* coor , double v );
    void setCellValue( unsigned int grid_x , unsigned int grid_y , double v);

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

private:
    QVector< QVector<double> > values;
};

Q_DECLARE_METATYPE(GWSGrid*)

#endif // GWSGRID_H
