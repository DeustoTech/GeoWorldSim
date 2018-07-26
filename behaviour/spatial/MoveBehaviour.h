#ifndef MOVEBEHAVIOUR_H
#define MOVEBEHAVIOUR_H

#include "../behaviour/Behaviour.h"

class MoveBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    MoveBehaviour( GWSAgent* behaving_agent );

    // PROPERTIES
    static QString DESTINATION_X;
    static QString DESTINATION_Y;

    // GETTERS
    virtual bool finished();

private slots:
    virtual bool behave();

};

#endif // MOVEBEHAVIOUR_H
