#ifndef CHECKIFATOTHERAGENTSPOSITIONBEHAVIOUR_H
#define CHECKIFATOTHERAGENTSPOSITIONBEHAVIOUR_H

#include <QObject>
#include "../Behaviour.h"

class CheckIfAtOtherAgentsPositionBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CheckIfAtOtherAgentsPositionBehaviour();

    // PROPERTIES
    static QString OTHER_AGENT_ID_PROP;

    virtual bool canContinueToNext();

private slots:
    virtual bool behave();
};

#endif // CHECKIFATOTHERAGENTSPOSITIONBEHAVIOUR_H
