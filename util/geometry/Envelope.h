#ifndef GWSENVELOPE_H
#define GWSENVELOPE_H

#include <QImage>
#include <QColor>

#include "../../util/units/Units.h"
#include "../../util/geometry/Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSPolygon)

class GWSEnvelope
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSEnvelope( QObject* parent = Q_NULLPTR );

    // PROPERTIES
    static QString LEFT_PROP;
    static QString RIGHT_PROP;
    static QString BOTTOM_PROP;
    static QString TOP_PROP;

    // EXPORTERS
    QImage toImage( const GWSEnvelope image_bounds , int image_width = 1024 , int image_height = 1024 , QColor border_color = QColor("black") , int border_width = 10 , QColor fill_color = QColor("green") ) const;
    QString toWKT() const;

    // GETTERS
    double getMaxX() const;
    double getMinX() const;
    double getMaxY() const;
    double getMinY() const;
    //GeoCoordinates getTopLeft() const;
    //GeoCoordinates getBottomLeft() const;
    //GeoCoordinates getTopRight() const;
    //GeoCoordinates getBottomRight() const;
    //GeoCoordinates getCentroid() const;
    //GeoCoordinates getRandomCoordinate() const;
    GWSAreaUnit getArea() const;
    bool covers( GWSEnvelope* env ) const;
    bool covers( GWSCoordinate* coor ) const;
    bool contains( GWSCoordinate* coor ) const;
    bool contains( GWSEnvelope* env ) const;
    bool intersects( GWSEnvelope* env ) const;
    void expandToInclude(GWSCoordinate* coor);

    // SETTERS
    void setMaxX( double maxX );
    void setMinX( double minX );
    void setMaxY( double maxY );
    void setMinY( double minY );

    // METHODS
    //GWSEnvelope createBuffer(double buffer) const;
    //GWSEnvelope createUnion(GWSEnvelope other) const;
    //GWSPolygon* createPolygon() const;

    // OPERATORS
    GWSEnvelope& operator= (const GWSEnvelope&);
    bool operator == (const GWSEnvelope&) const;
    bool operator != (const GWSEnvelope&) const;

};

Q_DECLARE_METATYPE(GWSEnvelope*)

#endif // GWSENVELOPE_H
