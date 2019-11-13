#ifndef GENERATEWASTEBEHAVIOUR_H
#define GENERATEWASTEBEHAVIOUR_H

#include "../../behaviour/Behaviour.h"

namespace geoworldsim {
namespace behaviour {

class GenerateWasteBehaviour : public Behaviour
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GenerateWasteBehaviour();

    // PROPERTIES

    static QString GENERATION; // Object with key-value per waste_category or sorting_type
    static QString SORTING_TYPES_CHARACTERIZATION; // Object of objects describing sorting type waste_categories and their amount (No matter if absolute or percentage)
    static QString STORE_GENERATED_WASTE_AS;
    static QString NEXTS;

protected slots:
    virtual QPair< double , QJsonArray > behave();

private:
    void generateForFullSortingType(QSharedPointer< Entity > entity , QJsonObject sorting_type_data , unit::MassUnit sorting_type_total , QString sorting_type , unit::MassUnit sorting_type_generate_amount);
    void generateForSortingTypeWasteCaregory( QSharedPointer< Entity > entity , QString sorting_type , QString waste_category , unit::MassUnit generate_amount );
};

}
}

#endif // GENERATEWASTEBEHAVIOUR_H
