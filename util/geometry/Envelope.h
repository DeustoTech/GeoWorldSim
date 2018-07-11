#ifndef GWSENVELOPE_H
#define GWSENVELOPE_H

#include "object/Object.h"
#include <QJsonArray>
#include <QImage>
#include <QColor>

#include "util/units/Units.h"
#include "util/geometry/Coordinate.h"

QT_FORWARD_DECLARE_CLASS(GWSPolygon)


class GWSEnvelope : public GWSObject
{
    Q_OBJECT

public:
    explicit GWSEnvelope();
    GWSEnvelope( double minX , double maxX , double minY , double maxY );
    GWSEnvelope( const GWSEnvelope& );

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
    GWSCoordinate getTopLeft() const;
    GWSCoordinate getBottomLeft() const;
    GWSCoordinate getTopRight() const;
    GWSCoordinate getBottomRight() const;
    GWSCoordinate getCentroid() const;
    GWSCoordinate getRandomCoordinate() const;
    GWSAreaUnit getArea() const;
    bool covers( GWSEnvelope env ) const;
    bool covers( GWSCoordinate coor ) const;
    bool contains( GWSCoordinate coor ) const;
    bool contains( GWSEnvelope env ) const;
    bool intersects( GWSEnvelope env ) const;
    void expandToInclude(GWSCoordinate coor);

    // METHODS
    GWSEnvelope createBuffer(double buffer) const;
    GWSEnvelope createUnion(GWSEnvelope other) const;
    GWSPolygon* createPolygon() const;

    // OPERATORS
    GWSEnvelope& operator= (const GWSEnvelope&);
    bool operator == (const GWSEnvelope&) const;
    bool operator != (const GWSEnvelope&) const;

};

Q_DECLARE_METATYPE(GWSEnvelope)

#endif // GWSENVELOPE_H
