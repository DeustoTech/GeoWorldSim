#ifndef GWSENTITYSOURCE_H
#define GWSENTITYSOURCE_H

#include <QObject>
#include <../../entity/Entity.h>

class GWSEntitySource : public GWSEntity
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GWSEntitySource();
    //~GWSAgentSource();

    // PROPERTIES
    static QString AMOUNT_PER_SECOND_PROP;
};

#endif // GWSENTITYSOURCE_H
