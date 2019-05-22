#ifndef GWSPROPERTYSTATISTICSBEHAVIOUR_H
#define GWSPROPERTYSTATISTICSBEHAVIOUR_H

#include "../../util/geometry/Coordinate.h"
#include "../../behaviour/Behaviour.h"

class SendPropertyStatisticsBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SendPropertyStatisticsBehaviour();

    // PROPERTIES
    static QString AGENTS_TYPE;
    static QString AGENTS_FILTER;
    static QString AGENTS_PROPERTY_NAME;
    static QString SOCKET_ID;
    static QString STORE_AS;
    static QString NEXTS_IF_ALIVE;
    static QString NEXTS_IF_DEAD;

private slots:

    virtual QPair< double , QJsonArray > behave();

private:
    QList<GWSCoordinate> previous_sent_coordinates;

};

#endif // GWSPROPERTYSTATISTICSBEHAVIOUR_H
