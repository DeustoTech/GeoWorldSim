#ifndef GWSAGENTSSOURCE_H
#define GWSAGENTSSOURCE_H

#include "Agent.h"

class GWSAgentsSource : public GWSAgent
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GWSAgentsSource( QObject* parent = Q_NULLPTR );

    // PROPERTIES
    static QString CREATION_TYPE_PROP;
    static QString CREATEION_RATIO_PROP; // Agents per second
    static QString CREATION_LIMIT_PROP; // Limit amount
    static QString CREATED_AMOUNT_PROP; // Limit amount

private slots: // SLOTS
    virtual void behave();

};

#endif // GWSAGENTSSOURCE_H
