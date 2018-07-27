#ifndef GWSGRID_H
#define GWSGRID_H

#include <QVector>
#include <QJsonObject>
#include <QImage>

QT_FORWARD_DECLARE_CLASS(GWSAgent)

class GWSGrid
{
    //[xy]
    //[03 , 13 , 23 , 33]
    //[02 , 12 , 22 , 32]
    //[01 , 11 , 21 , 31]
    //[00 , 10 , 20 , 30]

public:
    // CONSTRUCTORS
    GWSGrid( GWSAgent* agent );

    // PROPERTIES
    static QString GRID_MAX_VALUE_PROP;
    static QString GRID_MIN_VALUE_PROP;
    static QString GRID_VALUES_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;
    //QImage toImage( const GWSEnvelope image_bounds , int image_width = 1024, int image_height = 1024 ) const;

    // GETTERS
    bool isGridEmpty() const;
    unsigned int getGridXSize() const;
    unsigned int getGridYSize() const;
    double getGridMaxValue() const;
    double getGridMinValue() const;
    double getGridCellValue( unsigned int grid_x , unsigned int grid_y ) const;

    // SETTERS
    void setGridMaxValue( double max );
    void setGridMinValue( double min );
    void setGridSize( unsigned int x_size = 100 , unsigned int y_size = 100 );
    //void setCellValue( GWSCoordinate* coor , double v );
    void setGridCellValue( unsigned int grid_x , unsigned int grid_y , double v);

    // OPERATORS
    GWSGrid* operator+(const double number);
    GWSGrid* operator++();

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

private:

    // REG AGENT
    GWSAgent* agent;

    double max_value;
    double min_value;
    QVector< QVector<double> > values;
};

Q_DECLARE_METATYPE(GWSGrid*)

#endif // GWSGRID_H
