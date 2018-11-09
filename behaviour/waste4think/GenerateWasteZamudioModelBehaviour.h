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
    static QString WASTE_TYPE1;
    static QString WASTE_TYPE2;
    static QString WASTE_TYPE3;
    static QString WASTE_TYPE4;
    static QString GENERATED_WASTE_AMOUNT;
    static QString SAVE_GENERATED_WASTE_AS;
    static QString FAMILY_MEMBERS;
    static QString MAX_VALUE;
    static QString NEXTS;
    static QString STORE_GENERATED_WASTE_AS;
    static QString STORE_WASTE_TYPE1_AS;
    static QString STORE_WASTE_TYPE2_AS;
    static QString STORE_WASTE_TYPE3_AS;
    static QString STORE_WASTE_TYPE4_AS;



    // METHODS:
    double partialModel( double rest, double uni, double tasParo , double urb );

protected slots:
    virtual QStringList behave();
};

#endif // GENERATEWASTEBEHAVIOUR_H
