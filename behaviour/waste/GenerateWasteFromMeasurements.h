#ifndef GENERATEWASTEFROMMEASUREMENTS_H
#define GENERATEWASTEFROMMEASUREMENTS_H

#include "../../behaviour/Behaviour.h"

class GenerateWasteFromMeasurements : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GenerateWasteFromMeasurements();

    // PROPERTIES
    static QString MEASURED_WASTE;
    static QString GENERATED_WASTE_AMOUNT;
    static QString SAVE_GENERATED_WASTE_AS;
    static QString FAMILY_MEMBERS;
    static QString MAX_VALUE;
    static QString NEXTS_IF_TRUE;
    static QString NEXTS_IF_FALSE;
    static QString STORE_GENERATED_WASTE_AS;

protected slots:
    virtual QStringList behave();
};

#endif // GENERATEWASTEFROMMEASUREMENTS_H
