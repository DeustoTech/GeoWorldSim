#ifndef FINDDIRECTCLOSESTBEHAVIOUR_H
#define FINDDIRECTCLOSESTBEHAVIOUR_H


#include "../Behaviour.h"

class FindDirectClosestBehaviour : public GWSBehaviour
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit FindDirectClosestBehaviour();
    ~FindDirectClosestBehaviour();

    // PARAMETERS:
    static QString ENTITY_TO_ACCESS_TYPE;
    static QString STORE_DIRECT_CLOSEST_ID_AS;
    static QString STORE_DIRECT_CLOSEST_GEOM_AS;
    static QString STORE_DIRECT_DISTANCE_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_DIRECT_CLOSEST_FOUND;

protected:
     virtual QPair< double , QJsonArray > behave();
  //  GWSCoordinate closest_coor;
  //  QPair< GWSCoordinate, QString > closest_coor_id;
};

#endif // FINDDIRECTCLOSESTBEHAVIOUR_H
