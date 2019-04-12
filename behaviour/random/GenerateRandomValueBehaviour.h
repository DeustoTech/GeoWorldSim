#ifndef GENERATERANDOMVALUEBEHAVIOUR_H
#define GENERATERANDOMVALUEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GenerateRandomValueBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GenerateRandomValueBehaviour();


    // PROPERTIES
    static QString STORE_RANDOM_VALUE_AS;
    static QString MULTIPLIER;
    static QString NEXTS;


protected slots:
     virtual QPair< double , QJsonArray > behave();
};

#endif // GENERATERANDOMVALUEBEHAVIOUR_H
