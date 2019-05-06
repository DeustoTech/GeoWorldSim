#ifndef GWSSTOREMULTIROUTESKILL_H
#define GWSSTOREMULTIROUTESKILL_H

#include "../../skill/Skill.h"
#include "../../util/geometry/Coordinate.h"

class GWSStoreMultiRouteSkill : public GWSSkill
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSStoreMultiRouteSkill();

    // PROPERTIES
    static QString STORE_NEXT_DESTINATION_X_VALUE;
    static QString STORE_NEXT_DESTINATION_Y_VALUE;
    static QString STORE_NEXT_DESTINATION_AGENT_ID_AS;
    static QString PENDING_ROUTE_DESTINATIONS;


    // METHODS
    void addDestination( QString agent_id , QJsonObject extra_properties = QJsonObject() );
    void addDestination( GWSCoordinate coor , QJsonObject extra_properties = QJsonObject() );

};

#endif // GWSSTOREMULTIROUTESKILL_H
