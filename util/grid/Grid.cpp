#include "Grid.h"

#include <QtMath>
#include <QColor>
#include <QDebug>

#include "../../object/ObjectFactory.h"
#include "../../util/grid/GridCoordinatesConversor.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../util/geometry/GeometryTransformators.h"
#include "../../util/geometry/GeometryComparators.h"

GWSGrid::GWSGrid(GWSGeometry bounds, unsigned int x_size, unsigned int y_size , QString grid_type) : QObject(){

    this->grid = new QMap< unsigned int , QMap< unsigned int , QJsonValue >* >();

    for(unsigned int i = 0 ; i < x_size ; i++){

        this->grid->insert( i , new QMap<unsigned int , QJsonValue>() );

        for(unsigned int j = 0 ; j < y_size ; j++){
            this->grid->value( i )->insert( j , QJsonValue() );
        }
    }

    this->grid_type = grid_type;
    this->x_size = x_size;
    this->y_size = y_size;
    this->grid_bounds = bounds;
    this->max_x = GWSGeometryGetters::getGeometryMaxX( this->grid_bounds );
    this->max_y = GWSGeometryGetters::getGeometryMaxY( this->grid_bounds );
    this->min_x = GWSGeometryGetters::getGeometryMinX( this->grid_bounds );
    this->min_y = GWSGeometryGetters::getGeometryMinY( this->grid_bounds );
}

GWSGrid::~GWSGrid(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSGrid::getGeoJSON() const{

    QJsonObject geojson;
    geojson.insert( "type" , "GeometryCollection" );
    QJsonArray geometries;

    // BOUNDS

    for(int i = 0 ; i < x_size ; i++){
        for(int j = 0 ; j < y_size ; j++ ){
            QJsonObject geometry;
            geometry.insert( "type" , "Point" );
            QJsonArray coordinates = {
                GWSGridCoordinatesConversor::x2lon( i+0.5 , min_x , max_x , x_size ) ,
                GWSGridCoordinatesConversor::y2lat( j+0.5 , min_y , max_y , y_size )
            };

            geometry.insert( "coordinates" , coordinates );

            QJsonObject properties;
            properties.insert( "value" , this->grid->value( i )->value( j ) );
            geometry.insert( "properties" , properties );

            geometries.append( geometry );
        }
    }

    geojson.insert( "geometries" , geometries );

    return geojson;
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSGrid::isGridEmpty() const{
    return this->grid->isEmpty();
}

GWSGeometry GWSGrid::getBounds() const{
    return this->grid_bounds;
}

unsigned int GWSGrid::getXSize() const{
    return this->x_size;
}

unsigned int GWSGrid::getYSize() const{
    return this->y_size;
}

double GWSGrid::getLon( double x ) const{
    return GWSGridCoordinatesConversor::x2lon( x , min_x , max_x , x_size );
}

double GWSGrid::getLat(double y) const{
    return GWSGridCoordinatesConversor::y2lat( y , min_y , max_y , y_size );
}

double GWSGrid::getMaxValue() const{
    return this->max_value;
}

double GWSGrid::getMinValue() const{
    return this->min_value;
}

QJsonValue GWSGrid::getValue( const GWSCoordinate &coor ) const{

    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor.getX() , min_x , max_x , x_size );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor.getY() , min_y , max_y , y_size );

    if( this->grid->value( x ) ){
        return this->grid->value( x )->value( y , QJsonValue() );
    }
    return QJsonValue();
}

QJsonValue GWSGrid::getValue( const GWSGeometry &geom ) const{
    GWSCoordinate centroid = geom.getCentroid();
    return this->getValue( centroid );
}

/*double GWSGrid::getCellValue(GWSCoordinate* coor) const{
    if( this->isEmpty() ){ return 0; }
    if( !this->getBounds()->covers( coor ) ){
        qWarning() << QString("Coordintate outside grid bounds");
        return this->getMinValue() - 1;
    }
    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor->getX() , this->getBounds()->getMinX() , this->getBounds()->getMaxX() , this->getXSize() );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor->getY() , this->getBounds()->getMinY() , this->getBounds()->getMaxY() , this->getYSize() );
    return this->getCellValue( x , y );
}*/

/*const GWSEnvelope GWSGrid::getCellEnvelope(unsigned int grid_x, unsigned int grid_y) const{
    double left =   GWSGridCoordinatesConversor::x2lon( grid_x , this->getBounds()->getMinX() , this->getBounds()->getMaxX() , this->getXSize() );
    double top =    GWSGridCoordinatesConversor::y2lat( grid_y+1 , this->getBounds()->getMinY() , this->getBounds()->getMaxY() , this->getYSize() );
    double right =  GWSGridCoordinatesConversor::x2lon( grid_x+1 , this->getBounds()->getMinX() , this->getBounds()->getMaxX() , this->getXSize() );
    double bottom = GWSGridCoordinatesConversor::y2lat( grid_y , this->getBounds()->getMinY() , this->getBounds()->getMaxY() , this->getYSize() );

    return GWSEnvelope( left , right , top , bottom );
}*/



/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGrid::addValue( const GWSCoordinate &coor , const QJsonValue &value ){

    if( value.isNull() ){ return; }

    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor.getX() , min_x , max_x , this->x_size );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor.getY() , min_y , max_y , this->y_size );
    if( x < 0 || x >= x_size || y < 0 || y >= y_size ){
        qWarning() << QString("Value outside grid bounds");
        return;
    }

    QJsonValue existing_value = this->grid->value( x )->value( y , QJsonValue() );

    // How we add the value depends on the type of grid:

    // The grid cells represent the total value of the cell:
    if ( this->grid_type == "total" ){
        QJsonValue sum = GWSObjectFactory::incrementValue( existing_value , value );
        this->grid->value( x )->insert( y , sum );
        this->max_value = qMax( this->max_value , sum.toDouble() );
        this->min_value = qMin( this->min_value , sum.toDouble() );
    }

    // The grid cells represent the latest value of the cell:
    if ( this->grid_type == "latest" ){
        this->grid->value( x )->insert( y , value );
        this->max_value = qMax( this->max_value , value.toDouble() );
        this->min_value = qMin( this->min_value , value.toDouble() );
    }

    // The grid cells return the maximum between the previous and the latest:
    if ( this->grid_type == "maximum" ){
        QJsonValue max = qMax( existing_value.toDouble() , value.toDouble() );
        this->grid->value( x )->insert( y , max );
        this->max_value = qMax( this->max_value , max.toDouble() );
        this->min_value = qMin( this->min_value , max.toDouble() );
    }

    // The grid cells return the minimum between the previous and the latest:
    if ( this->grid_type == "minimum" ){
        QJsonValue min = qMin( existing_value.toDouble() , value.toDouble() );
        this->grid->value( x )->insert( y , min );
        this->max_value = qMax( this->max_value , min.toDouble() );
        this->min_value = qMin( this->min_value , min.toDouble() );
    }

}

void GWSGrid::addValue( const GWSGeometry &geom , const QJsonValue &value ){
    if( geom.isValid() && !value.isNull() ){
        GWSCoordinate centroid = geom.getCentroid();
        this->addValue( centroid , value );
    }
}

void GWSGrid::setBounds( const GWSGeometry &bounds ){

    if( this->grid_bounds == bounds ){
        return;
    }

    QMap<GWSCoordinate , QJsonValue > old_values;

    // Before resizing the grid, retrieve the coordinates and values
    if( this->grid_bounds.isValid() ){
        for(unsigned int i = 0 ; i < this->getXSize() ; i++){
            for(unsigned int j = 0 ; j < this->getYSize() ; j++ ){
                GWSCoordinate coor = GWSCoordinate( this->getLon( i+0.5 ) , this->getLat( j+0.5 ) );
                QJsonValue val = this->getValue( coor );
                if( !val.isNull() ){
                    old_values.insert( coor , val );
                }
                this->grid->value( i )->insert( j , QJsonValue() );
            }

        }
    }

    // Set new bounds
    this->grid_bounds = bounds;
    this->max_x = GWSGeometryGetters::getGeometryMaxX( this->grid_bounds );
    this->max_y = GWSGeometryGetters::getGeometryMaxY( this->grid_bounds );
    this->min_x = GWSGeometryGetters::getGeometryMinX( this->grid_bounds );
    this->min_y = GWSGeometryGetters::getGeometryMinY( this->grid_bounds );

    // Put back the old values
    foreach(GWSCoordinate c , old_values.keys()) {
        this->addValue( c , old_values.value( c ) );
    }

}

/**********************************************************************
 OPERATORS
**********************************************************************/

/*GWSGrid* GWSGrid::operator +(double number){
    for(int i = 0; i < this->values.size() ; i++) {
        for( int j = 0; j < this->values[i].size() ; j++){
            double value = this->getGridCellValue( i , j );
            this->setGridCellValue( i , j , qMin( this->getGridMaxValue() , value+number ) );
        }
    }
    return this;
}

GWSGrid* GWSGrid::operator ++(){
    for(int i = 0; i < this->values.size() ; i++) {
        for( int j = 0; j < this->values[i].size() ; j++){
            double value = this->getGridCellValue( i , j );
            this->setGridCellValue( i , j , qMin( this->getGridMaxValue() , value++ ) );
        }
    }
    return this;
}*/


/**********************************************************************
 METHODS
**********************************************************************/

/*GWSGrid GWSGrid::getSubGrid(GWSEnvelope bounds){

    // Create a new GRID
    double x_per_cell = (this->getBounds()->getMaxX() - this->getBounds()->getMinX()) / this->getXSize();
    double y_per_cell = (this->getBounds()->getMaxY() - this->getBounds()->getMinY()) / this->getYSize();

    int x_size = (bounds.getMaxX() - bounds.getMinX()) / x_per_cell;
    int y_size = (bounds.getMaxY() - bounds.getMinY()) / y_per_cell;

    GWSGrid bounds_grid;
    bounds_grid.setBounds( bounds.getMinX() , bounds.getMaxX() , bounds.getMaxY() , bounds.getMinY() );
    bounds_grid.setSize( x_size , y_size );
    bounds_grid.setMinValue( this->getMinValue() );
    bounds_grid.setMaxValue( this->getMaxValue() );

    double min_value = 99999999999;
    double max_value = -99999999999;

    // Iterate tiff grid and only take the cells that are inside bounds grid
    for( unsigned int x = 0 ; x < this->getXSize() ; x++ ){
        for( unsigned int y = 0; y < this->getYSize() ; y++ ){
            GWSEnvelope cell_env = this->getCellEnvelope( x , y );
            if( bounds.covers( cell_env ) ){

                GWSCoordinate center = cell_env.getCentroid();

                double value = this->getCellValue( center );
                min_value = qMin( min_value , value );
                max_value = qMax( max_value , value );

                bounds_grid.setCellValue( center , value );
            }
        }
    }
    return bounds_grid;
}

QList<GWSEnvelope> GWSGrid::getSurroundingCells( GWSCoordinate coor ){
    QList<GWSEnvelope> cells;

    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor->getX() , this->getBounds()->getMinX() , this->getBounds()->getMaxX() , this->values.size() );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor->getY() , this->getBounds()->getMinY() , this->getBounds()->getMaxY() , this->values[0].size() );

    cells.append( this->getCellEnvelope( x-1, y+1 ) );
    cells.append( this->getCellEnvelope( x-1, y ) );
    cells.append( this->getCellEnvelope( x-1, y-1 ) );

    cells.append( this->getCellEnvelope( x, y+1 ) );
    cells.append( this->getCellEnvelope( x, y ) );
    cells.append( this->getCellEnvelope( x, y-1 ) );

    cells.append( this->getCellEnvelope( x+1, y+1 ) );
    cells.append( this->getCellEnvelope( x+1, y ) );
    cells.append( this->getCellEnvelope( x+1, y-1 ) );

    return cells;
}*/
