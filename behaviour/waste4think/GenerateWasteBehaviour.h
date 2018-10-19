#ifndef GENERATEWASTEBEHAVIOUR_H
#define GENERATEWASTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

class GenerateWasteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GenerateWasteBehaviour();

    // PROPERTIES
    static QString WASTE_TYPE;
    static QString GENERATED_WASTE_AMOUNT;
    static QString SAVE_GENERATED_WASTE_AS;
    static QString FAMILY_MEMBERS;
    static QString MAX_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;
    static QString STORE_GENERATED_WASTE_AS;

    // METHODS:
    double partialModel( double rest, double uni, double tasParo , double urb );

protected slots:
    virtual QStringList behave();
};

#endif // GENERATEWASTEBEHAVIOUR_H
