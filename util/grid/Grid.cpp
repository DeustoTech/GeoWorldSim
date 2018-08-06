#include "Grid.h"

#include <QColor>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QDebug>

#include "../../agent/Agent.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../util/grid/GridCoordinatesConversor.h"

QString GWSGrid::GRID_MAX_VALUE_PROP = "grid_max_value";
QString GWSGrid::GRID_MIN_VALUE_PROP = "grid_min_value";
QString GWSGrid::GRID_VALUES_PROP = "grid_values";
QString GWSGrid::GRID_X_SIZE_PROP = "grid_x_size";
QString GWSGrid::GRID_Y_SIZE_PROP = "grid_y_size";

GWSGrid::GWSGrid(GWSAgent *agent){
    this->agent = agent;
}

GWSGrid::~GWSGrid(){
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSGrid::deserialize(QJsonObject json){
    this->max_value = json.value( GRID_MAX_VALUE_PROP ).toDouble();
    this->min_value = json.value( GRID_MIN_VALUE_PROP ).toDouble();
    this->setGridSize( json.value( GRID_X_SIZE_PROP ).toInt() , json.value( GRID_Y_SIZE_PROP ).toInt() );
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSGrid::serialize() const{
    QJsonObject json;
    json.insert( GRID_MAX_VALUE_PROP , this->max_value );
    json.insert( GRID_MIN_VALUE_PROP , this->min_value );
    json.insert( GRID_X_SIZE_PROP , (int)this->x_size );
    json.insert( GRID_Y_SIZE_PROP , (int)this->y_size );

    QJsonObject geojson;
    geojson.insert( "type" , "GeometryCollection" );
    QJsonArray geometries;

    // BOUNDS
    double left =   GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getGeometryMinX();
    double right =  GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getGeometryMaxX();
    double top =    GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getGeometryMaxY();
    double bottom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getAgent() )->getGeometryMinY();

    for(int i = 0 ; i < this->getGridXSize() ; i++){
        for(int j = 0 ; j < this->getGridYSize() ; j++ ){
            QJsonObject geometry;
            geometry.insert( "type" , "Polygon" );
            QJsonArray coordinates;

            double lon1 = GWSGridCoordinatesConversor::x2lon( i , left , right , this->getGridXSize() );
            double lat1 = GWSGridCoordinatesConversor::y2lat( j , bottom , top , this->getGridYSize() );
            double lon2 = GWSGridCoordinatesConversor::x2lon( i+1 , left , right , this->getGridXSize() );
            double lat2 = GWSGridCoordinatesConversor::y2lat( j+1 , bottom , top , this->getGridYSize() );

            // COOR1
            QJsonArray coor1; coor1 << lon1 << lat1;
            QJsonArray coor2; coor2 << lon1 << lat2;
            QJsonArray coor3; coor3 << lon2 << lat2;
            QJsonArray coor4; coor4 << lon2 << lat1;

            coordinates << coor1 << coor2 << coor3 << coor4 << coor1;

            QJsonArray polygons; polygons << coordinates;
            geometry.insert( "coordinates" , polygons );

            QJsonObject properties; properties.insert( "color" , "#00ff00" );
            geometry.insert( "properties" , properties );

            geometries.append( geometry );
        }
    }
    geojson.insert( "geometries" , geometries );
    json.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , geojson );

    return json;
}

/**********************************************************************
 GETTERS
**********************************************************************/

GWSAgent* GWSGrid::getAgent() const{
    return this->agent;
}

bool GWSGrid::isGridEmpty() const{
    return true;
}

/*const GWSEnvelope GWSGrid::getBounds() const{
    //return this->property( BOUNDS_PROP.toLatin1() ).value<GWSEnvelope*>();
}*/

int GWSGrid::getGridXSize() const{
    return this->x_size;
}

int GWSGrid::getGridYSize() const{
    return this->y_size;
}

/*const void* GWSGrid::getGridCellValue(unsigned int grid_x, unsigned int grid_y) const{
    if( grid_x >= this->getGridXSize() || grid_y >= this->getGridYSize() ){
        return Q_NULLPTR;
    }
    return this->values[grid_x][grid_y];
}*/

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

double GWSGrid::getGridMaxValue() const {
    return this->max_value;
}

double GWSGrid::getGridMinValue() const{
    return this->min_value;
}

double GWSGrid::getGridCellValue(int grid_x, int grid_y) const{
    Q_UNUSED( grid_x );
    Q_UNUSED( grid_y );
    return 0;
}

/**********************************************************************
 SETTERS
**********************************************************************/

/*void GWSGrid::setBounds(GWSEnvelope* bounds){
    //this->setProperty( GWSGrid::BOUNDS_PROP.toLatin1() , bounds );
}*/

void GWSGrid::setGridMaxValue(double max){
    this->max_value = max;
}

void GWSGrid::setGridMinValue(double min){
    this->min_value = min;
}

void GWSGrid::setGridSize(unsigned int x_size, unsigned int y_size){
    this->x_size = x_size;
    this->y_size = y_size;
}

/*void GWSGrid::setGridCellValue(unsigned int grid_x, unsigned int grid_y, void* v){
    this->values[grid_x][grid_y] = v;
}*/

/*void GWSGrid::setCellValue(GWSCoordinate* coor, double v){
    if( !this->getBounds()->contains( coor ) ){
        qWarning() << QString("Coordintate outside grid bounds");
        return;
    }
    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor->getX() , this->getBounds()->getMinX() , this->getBounds()->getMaxX() , this->values.size() );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor->getY() , this->getBounds()->getMinY() , this->getBounds()->getMaxY() , this->values[0].size() );
    this->setCellValue( x , y , v );
}*/


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
