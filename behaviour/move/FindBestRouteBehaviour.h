#ifndef FINDBESTROUTEBEHAVIOUR_H
#define FINDBESTROUTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class FindBestRouteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit FindBestRouteBehaviour();

    // GETTERS
    virtual bool continueToNext();

private slots:
    virtual bool behave();

};

#endif // FINDBESTROUTEBEHAVIOUR_H
