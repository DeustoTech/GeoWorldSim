#include "GenerateWasteFromMeasurements.h"

#include "../../util/datasource/DatasourceReader.h"


QString GenerateWasteFromMeasurements::MEASURED_WASTE = "kg_vivienda";
QString GenerateWasteFromMeasurements::FAMILY_MEMBERS = "family_members";
QString GenerateWasteFromMeasurements::MAX_VALUE = "max_waste";
QString GenerateWasteFromMeasurements::NEXTS_IF_TRUE = "nexts_if_true";
QString GenerateWasteFromMeasurements::NEXTS_IF_FALSE = "nexts_if_false";
QString GenerateWasteFromMeasurements::STORE_GENERATED_WASTE_AS = "store_generated_waste_as";

GenerateWasteFromMeasurements::GenerateWasteFromMeasurements() : GWSBehaviour(){

}


QStringList GenerateWasteFromMeasurements::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    double measured_waste = agent->getProperty( "kg_vivienda").toDouble(); // Daily generated waste amount




}



