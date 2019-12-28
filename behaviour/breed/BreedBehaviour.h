#ifndef BREEDBEHAVIOUR_H
#define BREEDBEHAVIOUR_H

#include "../Behaviour.h"

class BreedBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit BreedBehaviour();

    virtual bool finished();

private slots:
    virtual bool behave();
};

#endif // BREEDBEHAVIOUR_H
