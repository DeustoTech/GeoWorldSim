#ifndef QUADTREE_H
#define QUADTREE_H

#include <QObject>
#include <QReadWriteLock>
#include <QMap>

#include <geos/index/quadtree/Quadtree.h>

#include "../../util/geometry/Coordinate.h"
#include "../../util/geometry/Geometry.h"

namespace geoworldsim {
namespace geometry {


class Quadtree : public QObject
{
    Q_OBJECT

public:
    Quadtree();
    ~Quadtree();

    // GETTERS
    QStringList getElements() const;
    const Geometry getBounds() const;
    const Geometry getGeometry( const QString &object_id ) const;

    QStringList getElements( const Coordinate &coordinate );
    QStringList getElements( const Geometry &geom );
    QStringList getElements( double minX, double maxX, double minY, double maxY );

    QString getNearestElement( const Coordinate &coor );
    QString getNearestElement( const Geometry &geometry );


signals:

    void upsertGeometrySignal( const QString &object_id , const Geometry &coor );
    void upsertCoordinateSignal( const QString &object_id , const Coordinate &coor );
    void removeSignal( const QString &object_id );

public slots:

    // SETTERS
    void upsertCoordinate( const QString &object_id , const Coordinate &coor );
    void upsertGeometry( const QString &object_id , const Geometry &geom );
    void remove( const QString &object_id );

protected:

    std::string createHash( double value , int decimal_amount ) const;

private:

    // HELPER CLASS
    class QuadtreeElement {
    public:
        QuadtreeElement( std::string object_id , const Geometry &geometry ) : object_id( object_id ) , geometry( geometry ) {}
        std::string object_id;
        Geometry geometry;
        geos::geom::Envelope envelope;
    };

    mutable QReadWriteLock mutex;

    QStringList ids_contained;
    QMap< std::string , Quadtree::QuadtreeElement* >* quadtree_elements; // QMAP< (XHASH + YHASH) , QuadTreeElement >
    QMap< std::string , geos::index::quadtree::Quadtree* >* quadtree_layers; // QMAP< (XHASH + YHASH) , QuadTree >
    const unsigned int layer_depth_amount = 5;
    int stored_amount = 0;

    // BOUNDS
    double min_x = 180;
    double max_x = -180;
    double min_y = 180;
    double max_y = -180;

};

}
}

Q_DECLARE_METATYPE(geoworldsim::geometry::Quadtree*)

#endif // QUADTREE_H
