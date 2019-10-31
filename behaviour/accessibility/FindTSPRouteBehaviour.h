#ifndef FINDTSPROUTEBEHAVIOUR_H
#define FINDTSPROUTEBEHAVIOUR_H

#include "../Behaviour.h"

class FindTSPRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit FindTSPRouteBehaviour();

    // PROPERTIES
    static QString ENTITY_TYPE_TO_ORDER;
    static QString TRANSPORT_NETWORK_TYPE;
    static QString STORE_TSP_ROUTE_IDS_AS;
    static QString STORE_TSP_ROUTE_DISTANCE_AS;
    static QString STORE_TSP_ROUTE_GEOM_AS;
    static QString NEXTS;
    static QString NEXTS_IF_NO_TSP_FOUND;

protected slots:

     virtual QPair< double , QJsonArray > behave();

};

#endif // FINDTSPROUTEBEHAVIOUR_H
