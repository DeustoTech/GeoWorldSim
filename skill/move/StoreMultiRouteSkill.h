#ifndef GWSSTOREMULTIROUTESKILL_H
#define GWSSTOREMULTIROUTESKILL_H

#include "../../skill/Skill.h"
#include "../../util/geometry/Geometry.h"
#include "../../util/geometry/Coordinate.h"

namespace geoworldsim {
namespace skill{

class StoreMultiRouteSkill : public Skill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit StoreMultiRouteSkill();

    // PROPERTIES
    static QString STORE_NEXT_DESTINATION;
    static QString STORE_NEXT_DESTINATION_AGENT_ID_AS;
    static QString PENDING_ROUTE_DESTINATIONS;


    // METHODS
    void addDestination( const QString& agent_id , const QJsonObject& extra_properties = QJsonObject() );
    void addDestination( const geometry::Coordinate& coor , const QJsonObject& extra_properties = QJsonObject() );
    void addDestination( const geometry::Geometry& geom , const QJsonObject& extra_properties = QJsonObject() );

private:
    void addDestination( const QJsonObject& geojson , const QJsonObject& extra_properties = QJsonObject() );

};

}
}

#endif // GWSSTOREMULTIROUTESKILL_H
