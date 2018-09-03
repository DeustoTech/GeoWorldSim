#ifndef DELETEBEHAVIOUR_H
#define DELETEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class DeleteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit DeleteBehaviour();

    virtual bool finished(); // Behaviour finished check

private slots:
    virtual bool behave();

private:
    bool deleted = false;
};

#endif // DELETEBEHAVIOUR_H
