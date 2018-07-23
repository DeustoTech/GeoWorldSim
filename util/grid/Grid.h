#ifndef GWSGRID_H
#define GWSGRID_H

#include "../../object/Object.h"
#include <QVector>
#include <QJsonObject>
#include <QImage>

class GWSGrid : public GWSObject
{
    Q_OBJECT

    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

public:
    // CONSTRUCTORS
    Q_INVOKABLE explicit GWSGrid();

    // PROPERTIES
    static QString MAX_VALUE_PROP;
    static QString MIN_VALUE_PROP;
    static QString VALUES_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //QImage toImage( const GWSEnvelope image_bounds , int image_width = 1024, int image_height = 1024 ) const;

    // GETTERS
    bool isEmpty() const;
    unsigned int getXSize() const;
    unsigned int getYSize() const;
    double getMaxValue() const;
    double getMinValue() const;
    double getCellValue( unsigned int grid_x , unsigned int grid_y ) const;

    // SETTERS
    //void setBounds( GWSEnvelope bounds );
    void setMaxValue( double max );
    void setMinValue( double min );
    void setSize( unsigned int x_size = 100 , unsigned int y_size = 100 );
    //void setCellValue( GWSCoordinate* coor , double v );
    void setCellValue( unsigned int grid_x , unsigned int grid_y , double v);

    // OPERATORS
    GWSGrid* operator+(const double number);
    GWSGrid* operator++();

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

private:
    QVector< QVector<double> > values;
};

Q_DECLARE_METATYPE(GWSGrid*)

#endif // GWSGRID_H
