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
    static QString AGENT_TO_ACCESS_TYPE;
    static QString STORE_DIRECT_CLOSEST_ID_AS;
    static QString STORE_DIRECT_CLOSEST_X_AS;
    static QString STORE_DIRECT_CLOSEST_Y_AS;
    static QString STORE_DIRECT_CLOSEST_DISTANCE_AS;
    static QString NEXT;
    static QString NEXTS_IF_NO_DIRECT_CLOSEST_FOUND;

protected:
    virtual QJsonArray behave();
  //  GWSCoordinate closest_coor;
  //  QPair< GWSCoordinate, QString > closest_coor_id;
};

#endif // FINDDIRECTCLOSESTBEHAVIOUR_H
