#ifndef WAITUNTILTIMEBEHAVIOUR_H
#define WAITUNTILTIMEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class WaitUntilTimeBehaviour : public Behaviour
{
    Q_OBJECT
public:

    Q_INVOKABLE explicit WaitUntilTimeBehaviour();

    // PROPERTIES
    static QString WAIT_UNTIL_YEAR;
    static QString WAIT_UNTIL_MONTH;
    static QString WAIT_UNTIL_DAY;
    static QString WAIT_UNTIL_HOUR;
    static QString WAIT_UNTIL_MINUTE;
    static QString WAIT_UNTIL_SECOND;
    static QString WAIT_FREQUENCY;
    static QString NEXTS;

protected slots:
     virtual QPair< double , QJsonArray > behave();

};

}
}

#endif // WAITUNTILTIMEBEHAVIOUR_H
