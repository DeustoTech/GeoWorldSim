#include "Point.h"

#include <QPainter>
#include <QPen>
#include <QJsonArray>

#include "geos/geom/GeometryFactory.h"
#include "../../util/conversors/image_coordinates/ImageCoordinatesConversor.h"

GWSPoint::GWSPoint(double x, double y, double z ) : GWSGeometry( ) {
    this->inner_geometry =
            geos::geom::GeometryFactory::getDefaultInstance()->createPoint(
                geos::geom::Coordinate( x , y , z ) );
}

GWSPoint::GWSPoint(geos::geom::Point *inner_geometry ) : GWSGeometry(){
    this->inner_geometry = inner_geometry;
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSPoint::deserialize(QJsonObject json){

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

QJsonObject GWSPoint::serialize() const{
    QJsonObject json_geometry;
    if( !this->inner_geometry ){ return json_geometry; }

    if( this->inner_geometry->getGeometryTypeId() == geos::geom::GEOS_POINT ){
        json_geometry.insert("type", QString("Point") );

        QJsonArray coordinates;
        const geos::geom::Coordinate* coor = this->inner_geometry->getCoordinate();
        if( coor ){
            coordinates.append( coor->x );
            coordinates.append( coor->y );
            coordinates.append( coor->z );
        }
        json_geometry.insert("coordinates", coordinates);
        return json_geometry;
    }
    return GWSGeometry::serialize();
}

/**********************************************************************
 GETTERS
**********************************************************************/

double GWSPoint::getX() const{
    return this->inner_geometry->getCoordinate()->x;
}

double GWSPoint::getY() const{
    return this->inner_geometry->getCoordinate()->y;
}

double GWSPoint::getZ() const{
    return this->inner_geometry->getCoordinate()->z;
}
