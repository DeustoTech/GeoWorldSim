#ifndef GWSPROPERTYSTATISTICSBEHAVIOUR_H
#define GWSPROPERTYSTATISTICSBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class PropertyStatisticsBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit PropertyStatisticsBehaviour();

    // PROPERTIES
    static QString AGENTS_TYPE;
    static QString AGENTS_PROPERTY_NAME;
    static QString STORE_TOTAL_AS;
    static QString NEXTS;

private slots:
    virtual QPair< double , QJsonArray > behave();

};

#endif // GWSPROPERTYSTATISTICSBEHAVIOUR_H
