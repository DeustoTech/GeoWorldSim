#ifndef GENERATEWASTEBEHAVIOUR_H
#define GENERATEWASTEBEHAVIOUR_H

#include <QJsonObject>
#include "../../behaviour/Behaviour.h"

class GenerateWasteZamudioModelBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GenerateWasteZamudioModelBehaviour();

    // PROPERTIES

    static QString GENERATED_WASTE_AMOUNT;
    static QString SAVE_GENERATED_WASTE_AS;
    static QString FAMILY_MEMBERS;
    static QString MAX_VALUE;
    static QString STORE_GENERATED_WASTE_AS;
    static QString NEXTS;

    // METHODS:
    double partialModel( double rest, double uni, double tasParo , double urb );

protected slots:
    virtual QPair< double , QJsonArray > behave();
};

#endif // GENERATEWASTEBEHAVIOUR_H
