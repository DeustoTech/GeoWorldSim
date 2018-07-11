#include "HeatMap.h"

#include <QPainter>
#include <QRgb>
#include <QPen>
#include <QBrush>
#include <QRadialGradient>

#include <QJsonObject>
#include <QJsonArray>

#include "utils/conversors/image_coordinates/ImageCoordinatesConversor.h"

GSSHeatMap::GSSHeatMap() : max_value(0) {

    // Create color palette
    this->palette.setColorAt(0.45, Qt::blue);
    this->palette.setColorAt(0.55, Qt::cyan);
    this->palette.setColorAt(0.65, Qt::green);
    this->palette.setColorAt(0.85, Qt::yellow);
    this->palette.setColorAt(1.0, Qt::red);

}

GSSHeatMap::~GSSHeatMap(){
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GSSHeatMap::toJSON(){
    QJsonArray array;
    foreach( GWSCoordinate coor , this->points.keys() ){
        QJsonArray c;
        c.append( coor.getY() );
        c.append( coor.getX() );
        c.append( this->points.value( coor , 0 ) );
        array.append( c );
    }
    QJsonObject obj;
    obj.insert( "values" , array );
    obj.insert( "maximum" , this->max_value );
    return obj;
}

QImage GSSHeatMap::toImage(const GWSEnvelope image_bounds, int image_width, int image_height) const{

    // Image to be returned
    QImage image = QImage( image_width , image_height , QImage::Format_ARGB32 );
    image.fill( QColor( 0 , 0 , 0 , 0 ) );

    if( this->points.isEmpty() ){ return image; }

    // Painter
    QPainter painter( &image );
    painter.setRenderHint( QPainter::Antialiasing );

    // Create intermediate image with black to white
    foreach( GWSCoordinate coor , this->points.keys() ){
        if( image_bounds.createBuffer( 0.1 ).contains( coor ) ){

            double value = this->points.value( coor , 0 );
            QPoint point = ImageCoordinatesConversor::reprojectPoint(
                        coor.getY(),
                        coor.getX(),
                        image_bounds.getMaxY(),
                        image_bounds.getMinY(),
                        image_bounds.getMaxX(),
                        image_bounds.getMinX(),
                        image_width,
                        image_height );


            int radius = 64;
            QRadialGradient gradient( point.x() , point.y() , radius*(value/this->max_value) );
            // Set color according to value
            gradient.setColorAt(0.0, QColor( 0 , 0, 0 , 255.0*(value/this->max_value) ));
            gradient.setColorAt(1.0, QColor( 0 , 0 , 0 , 0 ));

            painter.setPen( Qt::NoPen );
            painter.setBrush( gradient );
            painter.drawEllipse( point , radius*(value/this->max_value) , radius*(value/this->max_value) );
        }
    }

    for( int x = 0 ; x < image.width() ; x++ ){
        for( int y = 0 ; y < image.height() ; y++ ){

            // Alpha channel stored in that pixel (0 to 1)
            int alpha = qAlpha( image.pixel( x , y ) );
            if( alpha > 0 ){
                // Transform channel to color
                QColor palette_color = this->palette.getColorAt( alpha/255.0 );
                image.setPixel( x , y , qRgba( palette_color.red() , palette_color.green() , palette_color.blue() , alpha ) );
            }
        }
    }

    return image;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GSSHeatMap::addPoint(const GWSCoordinate &point , double weight ){
    if( !point.isNull() ){

        double v = this->points.value( point , 0 )+weight;
        this->max_value = qMax( this->max_value , v );
        this->points.insert( point , v );

    }
}
