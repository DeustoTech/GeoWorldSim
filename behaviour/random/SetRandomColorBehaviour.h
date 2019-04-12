#ifndef SETRANDOMCOLORBEHAVIOUR_H
#define SETRANDOMCOLORBEHAVIOUR_H

#include <QObject>
#include "../../behaviour/Behaviour.h"

class SetRandomColorBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit SetRandomColorBehaviour();

    // PROPERTIES
    static QString NEXTS;


protected slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // SETRANDOMCOLORBEHAVIOUR_H
