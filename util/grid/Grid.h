#ifndef GWSGRID_H
#define GWSGRID_H

#include <QVector>
#include <QJsonObject>
#include <QImage>
#include <QMutex>
#include <QSharedPointer>

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
    GWSGrid( QSharedPointer<GWSAgent> agent );
    ~GWSGrid();

    // PROPERTIES
    static QString GRID_MAX_VALUE_PROP;
    static QString GRID_MIN_VALUE_PROP;
    static QString GRID_VALUES_PROP;
    static QString GRID_X_SIZE_PROP;
    static QString GRID_Y_SIZE_PROP;

    // IMPORTERS
    virtual void deserialize(QJsonObject json);

    // EXPORTERS
    virtual QJsonObject serialize() const;

    // GETTERS
    QSharedPointer<GWSAgent> getAgent() const;
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
    virtual void setGridSize( unsigned int x_size = 100 , unsigned int y_size = 100 );
    //void setCellValue( GWSCoordinate* coor , double v );
    //virtual void addGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void removeGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v) = 0;
    //virtual void setGridCellValue( unsigned int grid_x , unsigned int grid_y , void* v);

    // OPERATORS
    //GWSGrid* operator+(const double number);
    //GWSGrid* operator++();

    // METHODS
    //GWSGrid getSubGrid( GWSEnvelope bounds );
    //QList<GWSEnvelope> getSurroundingCells( GWSCoordinate coor );

protected:

    QMutex mutex; // To manage concurrency

private:

    // REG AGENT
    QSharedPointer<GWSAgent> agent;

    double max_value = -1;
    double min_value = -1;
    unsigned int x_size = -1;
    unsigned int y_size = -1;
};

Q_DECLARE_METATYPE(GWSGrid*)

#endif // GWSGRID_H
