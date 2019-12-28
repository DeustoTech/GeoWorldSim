#ifndef GSSHEATMAPPALETTE
#define GSSHEATMAPPALETTE

#include <QtMath>
#include <QImage>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>

class GSSHeatMapPalette
{

public:
        GSSHeatMapPalette(){
            this->canvas = QImage( 1000 , 1 , QImage::Format_ARGB32);
            this->gradient = QLinearGradient(0, 0, 1000, 1);
        }

        void setColorAt(qreal index, const QColor &color){

            this->gradient.setColorAt( index, color );

            // Regenerate canvas
            QPainter painter( &this->canvas );
            painter.setBrush( this->gradient );
            painter.setPen(Qt::NoPen);
            painter.fillRect( this->canvas.rect() , this->gradient );

        }

        QColor getColorAt(qreal step) const{ // Index from 0 to 1
            int i = qMin( this->canvas.width()-1 , (int)( step * this->canvas.width()) );
            return this->canvas.pixel( i , 0 );
        }

public:

        QImage canvas;
        QLinearGradient gradient;
};

#endif // GSSHEATMAPPALETTE

