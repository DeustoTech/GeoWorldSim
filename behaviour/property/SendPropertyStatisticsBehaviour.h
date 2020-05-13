 #ifndef GWSPROPERTYSTATISTICSBEHAVIOUR_H
#define GWSPROPERTYSTATISTICSBEHAVIOUR_H

#include "../../util/geometry/Coordinate.h"
#include "../../behaviour/Behaviour.h"
#include "../../util/grid/Grid.h"

namespace geoworldsim {
namespace behaviour {

class SendPropertyStatisticsBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SendPropertyStatisticsBehaviour();

    // PROPERTIES
    static QString ENTITY_TYPE;
    static QString ENTITY_FILTER;
    static QString ENTITY_PROPERTY_NAME;
    static QString GRID_TYPE;
    static QString SOCKET_ID;
    static QString STORE_AS;
    static QString NEXTS_IF_STILL_ALIVE;
    static QString NEXTS_IF_ALL_DEAD;


private slots:

    virtual QPair< double , QJsonArray > behave();

private:

    // STORAGES
    QJsonValue accumulated_total;
    grid::Grid* accumulated_grid = Q_NULLPTR;

    QList<geometry::Coordinate> previous_sent_coordinates;
    QStringList previous_sent_coordinates_ids;

};

}}

#endif // GWSPROPERTYSTATISTICSBEHAVIOUR_H
