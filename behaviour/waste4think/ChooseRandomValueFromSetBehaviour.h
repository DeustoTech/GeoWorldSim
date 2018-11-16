#ifndef CHOOSERANDOMVALUEFROMSETBEHAVIOUR_H
#define CHOOSERANDOMVALUEFROMSETBEHAVIOUR_H

#include <QObject>
#include "../../behaviour/Behaviour.h"


class ChooseRandomValueFromSetBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit ChooseRandomValueFromSetBehaviour();


    // PARAMETERS:
    static QString NEXTS;
    static QString VALUE_SET;
    static QString AGENT_PROPERTY_TO_ASSIGN;


protected slots:
    virtual QJsonArray behave();

};

#endif // CHOOSERANDOMVALUEFROMSETBEHAVIOUR_H
