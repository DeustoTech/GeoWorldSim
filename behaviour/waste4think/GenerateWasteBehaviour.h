#ifndef GENERATEWASTEBEHAVIOUR_H
#define GENERATEWASTEBEHAVIOUR_H

#include <QJsonObject>
#include "../../behaviour/Behaviour.h"

class GenerateWasteBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GenerateWasteBehaviour();

    // PROPERTIES

    static QString DAILY_GENERATION_PER_WASTE_CATEGORY;
    static QString CHARACTERIZATION_PER_WASTE_CATEGORY_AND_SORTING_TYPE;
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
