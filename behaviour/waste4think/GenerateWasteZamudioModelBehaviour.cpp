#include "GenerateWasteZamudioModelBehaviour.h"

#include <QJsonDocument>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

QString GenerateWasteZamudioModelBehaviour::FAMILY_MEMBERS = "family_members";
QString GenerateWasteZamudioModelBehaviour::DAILY_GENERATION_PER_WASTE_CATEGORY = "daily_generation_per_waste_category";
QString GenerateWasteZamudioModelBehaviour::CHARACTERIZATION_PER_WASTE_CATEGORY_AND_SORTING_TYPE = "characterization_per_waste_category_and_sorting_type";
QString GenerateWasteZamudioModelBehaviour::NEXTS = "nexts";

GenerateWasteZamudioModelBehaviour::GenerateWasteZamudioModelBehaviour() : GWSBehaviour(){

    // Careful! These are the quantities obtained from a whole container.
    // Right now, the model assumes each household generates this HUGE amount of waste
    QString characterizationOLD = "{"
                                 " \"resto\" : {"
                                        " \"despilfarro_alimento\" : 23.82 ,"
                                        " \"no_despilfarro_alimento\" : 35.04 , "
                                        " \"jardineria\" : 18.83 , "
                                        " \"carton_envase\" : 8.13 , "
                                        " \"carton_no_envase\" : 14.64 , "
                                        " \"vidrio_envase\" : 21.57 , "
                                        " \"envases_plastico_rigido\" : 13.51 ,"
                                        " \"film_plastico_envase\" : 11.02 , "
                                        " \"film_plastico_no_envase_bolsas\" : 5.43 , "
                                        " \"plastico_no_envase\" : 13.04 , "
                                        " \"envases_ferricos\" : 3.58 , "
                                        " \"envases_no_ferricos\" : 0.85 , "
                                        " \"envases_composite\" : 5.18 , "
                                        " \"madera_envase\" : 0.89 , "
                                        " \"madera_otros\" : 1.58 , "
                                        " \"textiles\" : 11.69 , "
                                        " \"zapatos_complementos\" : 3.66 , "
                                        " \"textil_sanitario\" : 0.19 , "
                                        " \"pañales\" : 24.37 , "
                                        " \"residuos_peligrosos_hogar\" : 0.22 , "
                                        " \"raees_mezclados\" : 3.05 , "
                                        " \"voluminosos\" : 39.14 , "
                                        " \"finos_inorganicos\" : 5.16 , "
                                        " \"fraccion_tamizada_20mm\" : 19.68 , "
                                        " \"otros\" : 6.64 "
                                        "}"
                                        "}";

    QJsonDocument jsonCharacOLD = QJsonDocument::fromJson( characterizationOLD.toUtf8() );

    // Zamudio daily generation From W4T_M1_ZamudioEnero19.xlsx:
    QString daily_generation = "{"
                                 " \"food_waste\" : 0.031479 ,"
                                   " \"no_food_waste\" :  0.296196, "
                                   " \"gardening_waste\" : 0.001347 , "
                                   " \"pruning_waste\" : 0.0006331 , "
                                   " \"other_biodegradable_waste\" :   0.000167 , "
                                   " \"paper_packaging\" : 0.063929 , "
                                   " \"cardboard_packaging\" : 0.047395 ,"
                                   " \"paper_cardboard_no_packaging\" :  0.013998 , "
                                   " \"glass_packaging\" :  0.000258, "
                                   " \"glass_no_packaging\" : 0.024464 , "
                                   " \"rigid_plastic_packaging\" : 0.031199 , "
                                   " \"plastic_film_packaging\" :  0.031990 , "
                                   " \"plastic_film_no_packaging_and_waste_collection_plastic_bags\" : 0.004117 , "
                                   " \"plastic_no_packaging\" :   0.004225, "
                                   " \"ferrous_packaging\" :  0.004046, "
                                   " \"non_ferrous_packaging\" :   0.003699, "
                                   " \"ferrous_no_packaging\" :  0.000320, "
                                   " \"non_ferrous_no_packaging\" : 0.002912 , "
                                   " \"composite_packaging\" : 0.001760 , "
                                   " \"other_complex_packaging\" :  0.000899, "
                                   " \"wood_packaging\" : 0.000207 , "
                                   " \"other_wood\" :  0.005026, "
                                   " \"textiles\":  0.011136 , "
                                   " \"shoes_and_accesories\" :   0.000084, "
                                   " \"sanitary_textile\" : 0.081692, "
                                   " \"nappies\" : 0.000297 , "
                                   " \"batteries_accumulators\" :  0.000004 , "
                                   " \"other_hazardous_waste\" :   0.000103, "
                                   " \"mixed_WEE\" : 0.001949 , "
                                   " \"bulky_waste\" : 0.109173 , "
                                   " \"soil_stones_miscellaneous_inerts\" : 0.022003 , "
                                   " \"10_mm_sieved_fraction\" :  0.019761 , "
                                   " \"other_not_in_other_categories\" :  0.000147 "
                                "}";


    QJsonDocument jsonDailyGeneration = QJsonDocument::fromJson( daily_generation.toUtf8() );
    QJsonObject dailyGenerationObject = jsonDailyGeneration.object();
    this->setProperty( "daily_generation_kg_person" , dailyGenerationObject );

    // Zamudio characterization From W4T_M1_ZamudioEnero19.xlsx:
    QString characterization = QString::fromUtf8("{"
                               " \"residual\" : { "
                                       " \"food_waste\" : 0.0413 ,"
                                       " \"no_food_waste\" :  0.4153, "
                                       " \"gardening_waste\" :  0.00, "
                                       " \"pruning_waste\" :  0.00, "
                                       " \"other_biodegradable_waste\" :  0.00, "
                                       " \"paper_packaging\" :  0.0085, "
                                       " \"cardboard_packaging\" :  0.0164,"
                                       " \"paper_cardboard_no_packaging\" :  0.0185 , "
                                       " \"glass_packaging\" :  0.00, "
                                       " \"glass_no_packaging\" :  0.0332, "
                                       " \"rigid_plastic_packaging\" :  0.0341, "
                                       " \"plastic_film_packaging\" :  0.0450, "
                                       " \"plastic_film_no_packaging_and_waste_collection_plastic_bags\" :  0.0011, "
                                       " \"plastic_no_packaging\" :  0.0056, "
                                       " \"ferrous_packaging\" :  0.0034, "
                                       " \"non_ferrous_packaging\" :  0.0051, "
                                       " \"ferrous_no_packaging\" :  0.00, "
                                       " \"non_ferrous_no_packaging\" :  0.0042, "
                                       " \"composite_packaging\" :  0.00, "
                                       " \"other_complex_packaging\" :  0.0011, "
                                       " \"wood_packaging\" : 0.00, "
                                       " \"other_wood\" :  0.0044, "
                                       " \"textiles\" : 0.0164  , "
                                       " \"shoes_and_accesories\" : 0.00, "
                                       " \"sanitary_textile\" :  0.1206, "
                                       " \"nappies\" :  0.00, "
                                       " \"batteries_accumulators\" : 0.00 , "
                                       " \"other_hazardous_waste\" :  0.00, "
                                       " \"mixed_WEE\" :  0.0022, "
                                       " \"bulky_waste\" :  0.1611, "
                                       " \"soil_stones_miscellaneous_inerts\" : 0.0332 , "
                                       " \"10_mm_sieved_fraction\" :  0.0293 , "
                                       " \"other_not_in_other_categories\" : 0.00 "
                                "} ,"
                                 " \"paper\" : { "
                                     " \"food_waste\" : 0.0256 ,"
                                     " \"no_food_waste\" :    0.0222, "
                                     " \"gardening_waste\" :  0.0115 , "
                                     " \"pruning_waste\" :  0.00 , "
                                     " \"other_biodegradable_waste\" :  0.0014 , "
                                     " \"paper_packaging\" :  0.4946 , "
                                     " \"cardboard_packaging\" : 0.3073 ,"
                                     " \"paper_cardboard_no_packaging\" :     0.0152, "
                                     " \"glass_packaging\" :  0.00 , "
                                     " \"glass_no_packaging\" :  0.0225, "
                                     " \"rigid_plastic_packaging\" : 0.0150 , "
                                     " \"plastic_film_packaging\" : 0.0029, "
                                     " \"plastic_film_no_packaging_and_waste_collection_plastic_bags\" : 0.0143 , "
                                     " \"plastic_no_packaging\" :  0.0018 , "
                                     " \"ferrous_packaging\" :    0.0006 , "
                                     " \"non_ferrous_packaging\" : 0.00 , "
                                     " \"ferrous_no_packaging\" : 0.0012, "
                                     " \"non_ferrous_no_packaging\" :  0.0015 , "
                                     " \"composite_packaging\" :  0.0001 , "
                                     " \"other_complex_packaging\" :    0.0012 , "
                                     " \"wood_packaging\" : 0.0018 , "
                                     " \"other_wood\" :  0.0182  , "
                                     " \"textiles\" :  0.0026, "
                                     " \"shoes_and_accesories\" : 0.00 , "
                                     " \"sanitary_textile\" :  0.0178 , "
                                     " \"nappies\" :  0.00 , "
                                     " \"batteries_accumulators\" :  0.00 , "
                                     " \"other_hazardous_waste\" :   0.0006 , "
                                     " \"mixed_WEE\" :     0.0010 , "
                                     " \"bulky_waste\" :  0.0148 , "
                                     " \"soil_stones_miscellaneous_inerts\" :  0.0014 , "
                                     " \"10_mm_sieved_fraction\" :     0.0018 , "
                                     " \"other_not_in_other_categories\" :   0.0010 "
                              "} ,"
                                 " \"packaging\" : { "
                                         " \"food_waste\" : 0.0014 ,"
                                         " \"no_food_waste\" :    0.0037, "
                                         " \"gardening_waste\" :  0.00 , "
                                         " \"pruning_waste\" : 0.00 , "
                                         " \"other_biodegradable_waste\" :  0.00 , "
                                         " \"paper_packaging\" :  0.0088 , "
                                         " \"cardboard_packaging\" : 0.0249 ,"
                                         " \"paper_cardboard_no_packaging\" : 0.00, "
                                         " \"glass_packaging\" :  0.0137 , "
                                         " \"glass_no_packaging\" : 0.00, "
                                         " \"rigid_plastic_packaging\" :  0.3681 , "
                                         " \"plastic_film_packaging\" : 0.1103 , "
                                         " \"plastic_film_no_packaging_and_waste_collection_plastic_bags\" : 0.0883 , "
                                         " \"plastic_no_packaging\" :  0.0176, "
                                         " \"ferrous_packaging\" :   0.0923, "
                                         " \"non_ferrous_packaging\" : 0.0178, "
                                         " \"ferrous_no_packaging\" : 0.0093, "
                                         " \"non_ferrous_no_packaging\" :  0.00, "
                                         " \"composite_packaging\" :  0.0933, "
                                         " \"other_complex_packaging\" : 0.0017, "
                                         " \"wood_packaging\" : 0.00, "
                                         " \"other_wood\" :  0.00, "
                                         " \"textiles\" :  0.0014, "
                                         " \"shoes_and_accesories\" : 0.0045 , "
                                         " \"sanitary_textile\" :  0.00, "
                                         " \"nappies\" :   0.0158, "
                                         " \"batteries_accumulators\" :  0.00 , "
                                         " \"other_hazardous_waste\" :  0.0007, "
                                         " \"mixed_WEE\" :   0.0213, "
                                         " \"bulky_waste\" : 0.0853, "
                                         " \"soil_stones_miscellaneous_inerts\" : 0.0008 , "
                                         " \"10_mm_sieved_fraction\" :  0.0170 , "
                                         " \"other_not_in_other_categories\" :    0.0017 "
                                  "} ,"
                              " \"organic\" : { "
                                         " \"food_waste\" :  0.0557,"
                                         " \"no_food_waste\" :  0.8877, "
                                         " \"gardening_waste\" :  0.00 , "
                                         " \"pruning_waste\" :  0.0271, "
                                         " \"other_biodegradable_waste\" : 0.00 , "
                                         " \"paper_packaging\" : 0.0003, "
                                         " \"cardboard_packaging\" :  0.0005 ,"
                                         " \"paper_cardboard_no_packaging\" :  0.0018 , "
                                         " \"glass_packaging\" :  0.00 , "
                                         " \"glass_no_packaging\"0,1 :  0.0006 , "
                                         " \"rigid_plastic_packaging\" :  0.0056 , "
                                         " \"plastic_film_packaging\" : 0.0015, "
                                         " \"plastic_film_no_packaging_and_waste_collection_plastic_bags\" :   0.0020 , "
                                         " \"plastic_no_packaging\" : 0.0004 , "
                                         " \"ferrous_packaging\" :  0.0001 , "
                                         " \"non_ferrous_packaging\" :    0.00 , "
                                         " \"ferrous_no_packaging\" : 0.00 , "
                                         " \"non_ferrous_no_packaging\" : 0.0002 , "
                                         " \"composite_packaging\" :   0.00, "
                                         " \"other_complex_packaging\" :  0.0011, "
                                         " \"wood_packaging\" : 0.00, "
                                         " \"other_wood\" :  0.00 , "
                                         " \"textiles\" :  0.00 , "
                                         " \"shoes_and_accesories\" :  0.00 , "
                                         " \"sanitary_textile\" :  0.0154 , "
                                         " \"nappies\" :  0.00, "
                                         " \"batteries_accumulators\" : 0.00, "
                                         " \"other_hazardous_waste\" :  0.00 , "
                                         " \"mixed_WEE\" :  0.00, "
                                         " \"bulky_waste\" :   0.00, "
                                         " \"soil_stones_miscellaneous_inerts\" :  0.00, "
                                         " \"10_mm_sieved_fraction\" : 0.00 , "
                                         " \"other_not_in_other_categories\" : 0.00"
                                  "} "
                                "}");

    QJsonDocument jsonCharacterization = QJsonDocument::fromJson( characterization.toUtf8() );
    QJsonObject characterizationObject = jsonCharacterization.object();
    this->setProperty( "characterization" , characterizationObject );
}

/************************************************************
 * METHODS
 ***********************************************************/

/* Formula for Zamudio waste generation based on Iraia's thesis:
   Data from UdalMap:
            |  2016 |  2015 |
      rest  |  9.56 |  9.52 |
      uni   | 20.16 | 20.13 |
    tasParo |  7.65 |  8.35 |
      urb   | 12.48 | 12.44 |*/


double GenerateWasteZamudioModelBehaviour::partialModel( double rest, double uni, double tasParo , double urb){

    // Fracción resto kg / habitante / año:

    double model = 344.57 + 2.56 * rest  + 1.44 * uni - 1.42 * tasParo - 1.01 * urb;
    return model;
}


QPair< double , QJsonArray > GenerateWasteZamudioModelBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();
    int family_members = entity->getProperty( this->getProperty( FAMILY_MEMBERS ).toString() ).toInt( 1 );

    QJsonObject generationObject = this->getProperty( DAILY_GENERATION_PER_WASTE_CATEGORY ).toObject( this->getProperty("daily_generation_kg_person" ).toObject());
    QJsonObject characterizationObject = this->getProperty( CHARACTERIZATION_PER_WASTE_CATEGORY_AND_SORTING_TYPE ).toObject( this->getProperty( "characterization" ).toObject());
    QJsonObject instant_generated = QJsonObject();
    QJsonObject accumulated_generated = entity->getProperty( "accumulated_generated" ).toObject();

    //qDebug() << characterizationObject.keys();

    // Check if the agent already has any waste stored:

    foreach( QString sorting_type , characterizationObject.keys() ){

        foreach (QString waste_category , characterizationObject.value( sorting_type ).toObject().keys() ) {

            double individual_waste_generated = generationObject.value( waste_category ).toDouble();

            QJsonValue charObj = characterizationObject.value( sorting_type ).toObject().value( waste_category );

            if ( charObj.isString() ){
                charObj = charObj.toString().toDouble();
            }

            double percentage_to_sorting_type = charObj.toDouble( 0 );
            GWSMassUnit generated_individual_waste_going_to_sorting_type = GWSMassUnit( individual_waste_generated * percentage_to_sorting_type * family_members);

            // STORE INSTANT PER SORTING TYPE

            // STORE ACCUMULATED PER SORTING TYPE
            double existing_value = entity->getProperty( "accumulated_" + sorting_type ).toDouble();
            entity->setProperty( "instant_" + sorting_type , generated_individual_waste_going_to_sorting_type.number() );
            entity->setProperty( "accumulated_" +  sorting_type , GWSObjectFactory::incrementValue( existing_value , generated_individual_waste_going_to_sorting_type.number() ) );

            // STORE INSTANT
            {
                QJsonObject copy = instant_generated[ sorting_type ].toObject();
                copy[ waste_category ] = generated_individual_waste_going_to_sorting_type.number();
                instant_generated[ sorting_type ] = copy;
            }

            // STORE ACCUMULATED
            {
                QJsonObject copy = accumulated_generated[ sorting_type ].toObject();
                copy[ waste_category ] = copy[ waste_category ].toDouble() +  generated_individual_waste_going_to_sorting_type.number();
                accumulated_generated[ sorting_type ] = copy;
            }
        }


    }

    entity->setProperty( "instant_generated" , instant_generated );
    entity->setProperty( "accumulated_generated" , accumulated_generated );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}


