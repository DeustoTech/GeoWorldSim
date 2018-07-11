#include "Grid.h"

#include <QJsonArray>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QDebug>

#include "util/geometry/GeometryFactory.h"

#include "util/conversors/image_coordinates/ImageCoordinatesConversor.h"
#include "util/grid/GridCoordinatesConversor.h"

/**
 *
 * @brief Grid::Grid
 * @param left
 * @param right
 * @param top
 * @param bottom
 * @param x_size
 * @param y_size
 * @param min_value
 * @param max_value
 * @param parent
 */
GWSGrid::GWSGrid(double left , double right , double top , double bottom , unsigned int x_size , unsigned int y_size , double min_value , double max_value ){
    this->setBounds( left , right , top , bottom );
    this->setSize( x_size , y_size );
    this->min_value = min_value;
    this->max_value = max_value;
}

GWSGrid::GWSGrid(const GWSGrid &other){
    this->setBounds( other.getBounds() );
    this->setSize( other.getXSize() , other.getYSize() );
    this->min_value = other.min_value;
    this->max_value = other.max_value;
    this->values = other.values;
}

GWSGrid::GWSGrid(){
}

GWSGrid::~GWSGrid(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QImage GWSGrid::toImage(const GWSEnvelope image_bounds, int image_width, int image_height) const{

    // Image to be retured
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    /*image.fill( QColor( 0 , 0 , 0 , 0 ) );

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );
    int num_points = 20;

    for( int i = 0 ; i < num_points ; i++ ){
        for( int j = 0 ; j < num_points ; j++ ){

            double lat_diff = (image_bounds.getMaxY() - image_bounds.getMinY()) / num_points;
            double lon_diff = (image_bounds.getMaxX() - image_bounds.getMinX()) / num_points;

            GWSCoordinate coor = GWSCoordinate( image_bounds.getMinX() + (j*lon_diff) , image_bounds.getMinY() + (i*lat_diff) );

            double value = this->getValue( coor );

            double diff_max = this->getMaxValue() - this->getMinValue();
            QPoint p = ImageCoordinatesConversor::reprojectPoint( coor.getY() , coor.getX() , image_bounds.getMaxY() , image_bounds.getMinY() , image_bounds.getMaxX() , image_bounds.getMinX() , image_width , image_height );

            if ( value == value && value <= this->getMaxValue() && value >= this->getMinValue() ){

                QColor fill = QColor( qFloor( 255 * (value / diff_max ) ) , qFloor( 255 * ( 1 - (value / diff_max) ) ) , 64 , 128 );
                QPen pen;
                pen.setColor( fill );
                pen.setWidth( image_width / num_points );
                painter.setPen( pen );
                painter.drawPoint( p );

            } else {

                QPen pen;
                pen.setColor( QColor("black") );
                pen.setWidth( 5 );
                painter.setPen( pen );
                painter.drawPoint( p );

            }
        }
    }*/

    /*int grid_min_x = GridCoordinatesConversor::lon2x( left , grid.getBounds().getMinX() , grid.getBounds().getMaxX() , grid.getXSize() );
    int grid_max_x = GridCoordinatesConversor::lon2x( right , grid.getBounds().getMinX() , grid.getBounds().getMaxX() , grid.getXSize() );
    int grid_min_y = GridCoordinatesConversor::lat2y( bottom , grid.getBounds().getMinY() , grid.getBounds().getMaxY() , grid.getYSize() );
    int grid_max_y = GridCoordinatesConversor::lat2y( top , grid.getBounds().getMinY() , grid.getBounds().getMaxY() , grid.getYSize() );

    for(int x = grid_min_x; x <= grid_max_x; x++){
        for(int y = grid_min_y; y <= grid_max_y; y++){

            geos::geom::Envelope envelope = grid.getCellEnvelope( x, y );

            geos::geom::Coordinate envelope_centroid;
            envelope.centre( envelope_centroid );
            double value = grid.getValue( envelope_centroid );

            double diff_max = grid.getMaxValue() - grid.getMinValue();
            double diff_value = grid.getMaxValue() - value;

            QColor fill;
            if ( value == value && value <= grid.getMaxValue() && value >= grid.getMinValue() ){
                fill = QColor( qFloor( 255 * 1 - ( diff_value / diff_max ) ) , qFloor( 255 * diff_value / diff_max ), qFloor( 255 * diff_value / diff_max ) );
                painter.drawImage( 0 , 0 , QImageExporter::format( envelope , image_bounds , image_width , image_height , fill , fill ) );
            } else {
                painter.drawImage( 0 , 0 , QImageExporter::format( envelope , image_bounds , image_width , image_height , QColor("black") , QColor( 0 , 0 , 0 , 0 ) ) );
            }
        }
    }*/
    return image;
}

/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSGrid::isEmpty() const{
    return this->values.isEmpty();
}

const GWSEnvelope GWSGrid::getBounds() const{
    return this->bounds;
}

unsigned int GWSGrid::getXSize() const{
    return this->values.size();
}

unsigned int GWSGrid::getYSize() const{
    if( this->values.isEmpty() ){ return 0; }
    return this->values.at(0).size();
}

double GWSGrid::getValue(unsigned int grid_x, unsigned int grid_y) const{
    return this->values[grid_x][grid_y];
}

double GWSGrid::getValue(GWSCoordinate coor) const{
    if( this->isEmpty() ){ return this->min_value - 1; }
    if( !this->bounds.covers( coor ) ){
        qWarning() << QString("Coordintate outside grid bounds");
        return this->min_value - 1;
    }
    unsigned int x = GridCoordinatesConversor::lon2x( coor.getX() , this->bounds.getMinX() , this->bounds.getMaxX() , this->getXSize() );
    unsigned int y = GridCoordinatesConversor::lat2y( coor.getY() , this->bounds.getMinY() , this->bounds.getMaxY() , this->getYSize() );
    return this->getValue( x , y );
}

const GWSEnvelope GWSGrid::getCellEnvelope(unsigned int grid_x, unsigned int grid_y) const{
    double left =   GridCoordinatesConversor::x2lon( grid_x , this->bounds.getMinX() , this->bounds.getMaxX() , this->getXSize() );
    double top =    GridCoordinatesConversor::y2lat( grid_y+1 , this->bounds.getMinY() , this->bounds.getMaxY() , this->getYSize() );
    double right =  GridCoordinatesConversor::x2lon( grid_x+1 , this->bounds.getMinX() , this->bounds.getMaxX() , this->getXSize() );
    double bottom = GridCoordinatesConversor::y2lat( grid_y , this->bounds.getMinY() , this->bounds.getMaxY() , this->getYSize() );

    return GWSEnvelope( left , right , top , bottom );
}

double GWSGrid::getMaxValue() const {
    return this->max_value;
}

double GWSGrid::getMinValue() const{
    return this->min_value;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSGrid::setBounds(double left, double right, double top, double bottom){
    this->bounds = GWSEnvelope(left , right , top , bottom);
}

void GWSGrid::setBounds(GWSEnvelope bounds){
    this->bounds = bounds;
}

void GWSGrid::setSize(unsigned int x_size, unsigned int y_size){
    this->values.clear();
    this->values = QVector< QVector<double> >( x_size , QVector<double>( y_size , NAN ) );
}

void GWSGrid::setValue(unsigned int grid_x, unsigned int grid_y, double v){
    if( v < this->min_value || v > this->max_value ){
        this->values[grid_x][grid_y] = NAN;
    } else {
        this->values[grid_x][grid_y] = v;
    }
}

void GWSGrid::setValue(GWSCoordinate coor, double v){
    if( !this->bounds.contains( coor ) ){
        qWarning() << QString("Coordintate outside grid bounds");
        return;
    }
    unsigned int x = GridCoordinatesConversor::lon2x( coor.getX() , this->bounds.getMinX() , this->bounds.getMaxX() , this->values.size() );
    unsigned int y = GridCoordinatesConversor::lat2y( coor.getY() , this->bounds.getMinY() , this->bounds.getMaxY() , this->values[0].size() );
    this->setValue( x , y , v );
}

/**********************************************************************
 OPERATORS
**********************************************************************/

GWSGrid& GWSGrid::operator=(const GWSGrid& other){
    this->bounds = other.bounds;
    this->values = other.values;
    this->min_value = other.min_value;
    this->max_value = other.max_value;
    return *this;
}

/**********************************************************************
 METHODS
**********************************************************************/

GWSGrid GWSGrid::getSubGrid(GWSEnvelope bounds){

    // Create a new GRID
    double x_per_cell = (this->getBounds().getMaxX() - this->getBounds().getMinX()) / this->getXSize();
    double y_per_cell = (this->getBounds().getMaxY() - this->getBounds().getMinY()) / this->getYSize();

    int x_size = (bounds.getMaxX() - bounds.getMinX()) / x_per_cell;
    int y_size = (bounds.getMaxY() - bounds.getMinY()) / y_per_cell;

    GWSGrid bounds_grid (bounds.getMinX() , bounds.getMaxX() , bounds.getMaxY() , bounds.getMinY() , x_size , y_size , this->getMinValue() , this->getMaxValue() );

    double min_value = 99999999999;
    double max_value = -99999999999;

    // Iterate tiff grid and only take the cells that are inside bounds grid
    for( unsigned int x = 0 ; x < this->getXSize() ; x++ ){
        for( unsigned int y = 0; y < this->getYSize() ; y++ ){
            GWSEnvelope cell_env = this->getCellEnvelope( x , y );
            if( bounds.covers( cell_env ) ){

                GWSCoordinate center = cell_env.getCentroid();

                double value = this->getValue( center );
                min_value = qMin( min_value , value );
                max_value = qMax( max_value , value );

                bounds_grid.setValue( center , value );
            }
        }
    }
    return bounds_grid;
}

QList<GWSEnvelope> GWSGrid::getSurroundingCells( GWSCoordinate coor ){
    QList<GWSEnvelope> cells;

    unsigned int x = GridCoordinatesConversor::lon2x( coor.getX() , this->bounds.getMinX() , this->bounds.getMaxX() , this->values.size() );
    unsigned int y = GridCoordinatesConversor::lat2y( coor.getY() , this->bounds.getMinY() , this->bounds.getMaxY() , this->values[0].size() );

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
}
