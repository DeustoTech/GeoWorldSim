#ifndef ENTITYSOURCE_H
#define ENTITYSOURCE_H

#include <QObject>
#include <../../entity/Entity.h>

namespace geoworldsim {


class EntitySource : public Entity
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit EntitySource();
    //~GWSAgentSource();

    // PROPERTIES
    static QString AMOUNT_PER_SECOND_PROP;
};

}

#endif // ENTITYSOURCE_H
