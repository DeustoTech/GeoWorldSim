#include "Grid.h"

#include <QtMath>
#include <QDebug>

#include "../../object/ObjectFactory.h"
#include "../../util/grid/GridCoordinatesConversor.h"
#include "../../util/geometry/GeometryGetters.h"
#include "../../util/geometry/GeometryTransformators.h"
#include "../../util/geometry/GeometryComparators.h"

GWSGrid::GWSGrid(GWSGeometry bounds, unsigned int x_size, unsigned int y_size) : QObject(){

    for(unsigned int i = 0 ; i < x_size ; i++){

        this->grid->insert( i , new QMap<unsigned int , QJsonValue>() );

        for(unsigned int j = 0 ; j < y_size ; j++){
            this->grid->value( i )->insert( j , QJsonValue() );
        }
    }

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

QJsonObject GWSGrid::serialize() const{

    QJsonObject json;

    QJsonObject geojson;
    geojson.insert( "type" , "GeometryCollection" );
    QJsonArray geometries;

    // BOUNDS
    /*GWSGeometry geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent );
    double left =   GWSGeometryGetters::getGeometryMinX( geom );
    double right =  GWSGeometryGetters::getGeometryMaxX( geom );
    double top =    GWSGeometryGetters::getGeometryMaxY( geom );
    double bottom = GWSGeometryGetters::getGeometryMinY( geom );

    double left =   GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMinX();
    double right =  GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMaxX();
    double top =    GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMaxY();
    double bottom = GWSPhysicalEnvironment::globalInstance()->getGeometry( agent )->getGeometryMinY();

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
    json.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , geojson );*/

    return json;
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

int GWSGrid::getXSize() const{
    return this->x_size;
}

int GWSGrid::getYSize() const{
    return this->y_size;
}

QJsonValue GWSGrid::getValue( GWSCoordinate coor ) const{

    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor.getX() , min_x , max_x , x_size );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor.getY() , min_y , max_y , y_size );
    return this->grid->value( x )->value( y , QJsonValue() );
}

QJsonValue GWSGrid::getValue( GWSGeometry geom ) const{
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


void GWSGrid::setValue( GWSCoordinate coor , QJsonValue value ){

    if( !GWSGeometryComparators::contains( this->getBounds() , coor ) ){
        qWarning() << QString("Value outside grid bounds");
        return;
    }

    unsigned int x = GWSGridCoordinatesConversor::lon2x( coor.getX() , min_x , max_x , this->x_size );
    unsigned int y = GWSGridCoordinatesConversor::lat2y( coor.getY() , min_y , max_y , this->y_size );
    QJsonValue existing_value = this->grid->value( x )->value( y , QJsonValue() );
    this->grid->value( x )->insert( y , GWSObjectFactory::incrementValue( existing_value , value ) );
}

void GWSGrid::setValue( GWSGeometry geom , QJsonValue value ){
    GWSCoordinate centroid = geom.getCentroid();
    this->setValue( centroid , value );
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
