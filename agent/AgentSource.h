#ifndef GWSAGENTSOURCE_H
#define GWSAGENTSOURCE_H

#include <QObject>
#include <../../agent/Agent.h>

class GWSAgentSource : public GWSAgent
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GWSAgentSource();
    //~GWSAgentSource();

    // PROPERTIES
    static QString AMOUNT_PER_SECOND_PROP;
};

#endif // GWSAGENTSOURCE_H
