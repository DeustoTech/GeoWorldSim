#ifndef GENERATERANDOMVALUEBEHAVIOUR_H
#define GENERATERANDOMVALUEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class GenerateRandomValueBehaviour : public Behaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GenerateRandomValueBehaviour();

    // PROPERTIES
    static QString STORE_AS;
    static QString MULTIPLIER;
    static QString NEXTS;


protected slots:
     virtual QPair< double , QJsonArray > behave();
};

}
}

#endif // GENERATERANDOMVALUEBEHAVIOUR_H
