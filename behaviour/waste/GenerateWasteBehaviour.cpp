#include "GenerateWasteBehaviour.h"

#include <QJsonDocument>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"

QString GenerateWasteBehaviour::DAILY_GENERATION = "daily_generation";
QString GenerateWasteBehaviour::SORTING_TYPES_CHARACTERIZATION = "characterization";
QString GenerateWasteBehaviour::NEXTS = "nexts";

GenerateWasteBehaviour::GenerateWasteBehaviour() : GWSBehaviour(){

    // Careful! These are the quantities obtained from a whole container.
    // Right now, the model assumes each household generates this HUGE amount of waste


    // Zamudio characterization From W4T_M1_ZamudioEnero19.xlsx:
    // Residual: ST2
    // Organic: ST10
    // Packaging: ST5
    // Paper: ST7
    QJsonObject default_characterization = QJsonObject({
                               { "SortingType:2" , QJsonObject({
                                       { "food_waste" , 0.0413 } ,
                                       { "no_food_waste" , 0.4153 } ,
                                       { "gardening_waste" , 0.00 } ,
                                       { "pruning_waste" , 0.00 } ,
                                       { "other_biodegradable_waste" , 0.00 } ,
                                       { "paper_packaging" , 0.0085 } ,
                                       { "cardboard_packaging" , 0.0164 } ,
                                       { "paper_cardboard_no_packaging" , 0.0185  } ,
                                       { "glass_packaging" , 0.00 } ,
                                       { "glass_no_packaging" , 0.0332 } ,
                                       { "rigid_plastic_packaging" , 0.0341 } ,
                                       { "plastic_film_packaging" , 0.0450 } ,
                                       { "plastic_film_no_packaging_and_waste_collection_plastic_bags" , 0.0011 } ,
                                       { "plastic_no_packaging" , 0.0056 } ,
                                       { "ferrous_packaging" , 0.0034 } ,
                                       { "non_ferrous_packaging" , 0.0051 } ,
                                       { "ferrous_no_packaging" , 0.00 } ,
                                       { "non_ferrous_no_packaging" , 0.0042 } ,
                                       { "composite_packaging" , 0.00 } ,
                                       { "other_complex_packaging" , 0.0011 } ,
                                       { "wood_packaging" , 0.00 } ,
                                       { "other_wood" , 0.0044 } ,
                                       { "textiles" , 0.0164  } ,
                                       { "shoes_and_accesories" , 0.00 } ,
                                       { "sanitary_textile" , 0.1206 } ,
                                       { "nappies" , 0.00 } ,
                                       { "batteries_accumulators" , 0.00  } ,
                                       { "other_hazardous_waste" , 0.00 } ,
                                       { "mixed_WEE" , 0.0022 } ,
                                       { "bulky_waste" , 0.1611 } ,
                                       { "soil_stones_miscellaneous_inerts" , 0.0332  } ,
                                       { "10_mm_sieved_fraction" , 0.0293  } ,
                                       { "other_not_in_other_categories" , 0.00  }
                                }) } ,

                                { "SortingType:7" , QJsonObject({
                                        { "food_waste" , 0.0256 } ,
                                        { "no_food_waste" ,    0.0222 } ,
                                        { "gardening_waste" ,  0.0115  } ,
                                        { "pruning_waste" ,  0.00  } ,
                                        { "other_biodegradable_waste" ,  0.0014  } ,
                                        { "paper_packaging" ,  0.4946  } ,
                                        { "cardboard_packaging" , 0.3073 } ,
                                        { "paper_cardboard_no_packaging" ,   0.0152 } ,
                                        { "glass_packaging" ,  0.00  } ,
                                        { "glass_no_packaging" ,  0.0225 } ,
                                        { "rigid_plastic_packaging" , 0.0150  } ,
                                        { "plastic_film_packaging" , 0.0029 } ,
                                        { "plastic_film_no_packaging_and_waste_collection_plastic_bags" , 0.0143  } ,
                                        { "plastic_no_packaging" ,  0.0018  } ,
                                        { "ferrous_packaging" ,    0.0006  } ,
                                        { "non_ferrous_packaging" , 0.00  } ,
                                        { "ferrous_no_packaging" , 0.0012 } ,
                                        { "non_ferrous_no_packaging" ,  0.0015  } ,
                                        { "composite_packaging" ,  0.0001  } ,
                                        { "other_complex_packaging" ,    0.0012  } ,
                                        { "wood_packaging" , 0.0018  } ,
                                        { "other_wood" ,  0.0182 } ,
                                        { "textiles" ,  0.0026 } ,
                                        { "shoes_and_accesories" , 0.00 } ,
                                        { "sanitary_textile" ,  0.0178 } ,
                                        { "nappies" ,  0.00  } ,
                                        { "batteries_accumulators" ,  0.00 } ,
                                        { "other_hazardous_waste" ,   0.0006 } ,
                                        { "mixed_WEE" ,     0.0010 } ,
                                        { "bulky_waste" ,  0.0148 } ,
                                        { "soil_stones_miscellaneous_inerts" ,  0.0014  } ,
                                        { "10_mm_sieved_fraction" ,     0.0018  } ,
                                        { "other_not_in_other_categories" ,   0.0010  }
                                    }) } ,
                                    { "SortingType:5" , QJsonObject({
                                            { "food_waste" , 0.0014 } ,
                                            { "no_food_waste" ,    0.0037 } ,
                                            { "gardening_waste" ,  0.00  } ,
                                            { "pruning_waste" , 0.00  } ,
                                            { "other_biodegradable_waste" ,  0.00  } ,
                                            { "paper_packaging" ,  0.0088  } ,
                                            { "cardboard_packaging" , 0.0249 } ,
                                            { "paper_cardboard_no_packaging" , 0.00 } ,
                                            { "glass_packaging" ,  0.0137  } ,
                                            { "glass_no_packaging" , 0.00 } ,
                                            { "rigid_plastic_packaging" ,  0.3681  } ,
                                            { "plastic_film_packaging" , 0.1103  } ,
                                            { "plastic_film_no_packaging_and_waste_collection_plastic_bags" , 0.0883  } ,
                                            { "plastic_no_packaging" ,  0.0176 } ,
                                            { "ferrous_packaging" ,   0.0923 } ,
                                            { "non_ferrous_packaging" , 0.0178 } ,
                                            { "ferrous_no_packaging" , 0.0093 } ,
                                            { "non_ferrous_no_packaging" ,  0.00 } ,
                                            { "composite_packaging" ,  0.0933 } ,
                                            { "other_complex_packaging" , 0.0017 } ,
                                            { "wood_packaging" , 0.00 } ,
                                            { "other_wood" ,  0.00 } ,
                                            { "textiles" ,  0.0014 } ,
                                            { "shoes_and_accesories" , 0.0045  } ,
                                            { "sanitary_textile" ,  0.00 } ,
                                            { "nappies" ,   0.0158 } ,
                                            { "batteries_accumulators" ,  0.00  } ,
                                            { "other_hazardous_waste" ,  0.0007 } ,
                                            { "mixed_WEE" ,   0.0213 } ,
                                            { "bulky_waste" , 0.0853 } ,
                                            { "soil_stones_miscellaneous_inerts" , 0.0008  } ,
                                            { "10_mm_sieved_fraction" ,  0.0170  } ,
                                            { "other_not_in_other_categories" ,    0.0017 }
                                 }) } ,
                                 { "SortingType:10" , QJsonObject({
                                            { "food_waste" ,  0.0557} ,
                                            { "no_food_waste" ,  0.8877 } ,
                                            { "gardening_waste" ,  0.00  } ,
                                            { "pruning_waste" ,  0.0271 } ,
                                            { "other_biodegradable_waste" , 0.00  } ,
                                            { "paper_packaging" , 0.0003 } ,
                                            { "cardboard_packaging" ,  0.0005 } ,
                                            { "paper_cardboard_no_packaging" ,  0.0018  } ,
                                            { "glass_packaging" ,  0.00  } ,
                                            { "glass_no_packaging" ,  0.0006  } ,
                                            { "rigid_plastic_packaging" ,  0.0056  } ,
                                            { "plastic_film_packaging" , 0.0015 } ,
                                            { "plastic_film_no_packaging_and_waste_collection_plastic_bags" ,   0.0020  } ,
                                            { "plastic_no_packaging" , 0.0004  } ,
                                            { "ferrous_packaging" ,  0.0001  } ,
                                            { "non_ferrous_packaging" ,    0.00  } ,
                                            { "ferrous_no_packaging" , 0.00  } ,
                                            { "non_ferrous_no_packaging" , 0.0002  } ,
                                            { "composite_packaging" ,   0.00 } ,
                                            { "other_complex_packaging" ,  0.0011 } ,
                                            { "wood_packaging" , 0.00 } ,
                                            { "other_wood" ,  0.00  } ,
                                            { "textiles" ,  0.00  } ,
                                            { "shoes_and_accesories" ,  0.00  } ,
                                            { "sanitary_textile" ,  0.0154  } ,
                                            { "nappies" ,  0.00 } ,
                                            { "batteries_accumulators" , 0.00 } ,
                                            { "other_hazardous_waste" ,  0.00  } ,
                                            { "mixed_WEE" ,  0.00 } ,
                                            { "bulky_waste" ,   0.00 } ,
                                            { "soil_stones_miscellaneous_inerts" ,  0.00 } ,
                                            { "10_mm_sieved_fraction" , 0.00  } ,
                                            { "other_not_in_other_categories" , 0.00 }
                                    }) }
                                });

    this->setProperty( this->getProperty( SORTING_TYPES_CHARACTERIZATION ).toString() , default_characterization );

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



QPair< double , QJsonArray > GenerateWasteBehaviour::behave(){

    QSharedPointer<GWSEntity> entity = this->getEntity();

    QJsonObject characterization_data = this->getProperty( SORTING_TYPES_CHARACTERIZATION ).toObject( this->getProperty( "characterization" ).toObject());

    // Accumulate the total per sorting_type and waste_category
    QMap< QString , GWSMassUnit > totals;

    foreach( QString sorting_type_name , characterization_data.keys() ) {

        totals[ sorting_type_name ] = 0;
        QJsonObject sorting_type_characterization = characterization_data[ sorting_type_name ].toObject();

        foreach( QString waste_category_name , sorting_type_characterization.keys() ) {
            double value = sorting_type_characterization.value( waste_category_name ).toDouble();
            totals[ waste_category_name ] = totals.value( waste_category_name , 0 ) + value;
            totals[ sorting_type_name ] += value;
        }
    }

    // Iterate the daily generation
    QJsonObject generation_data = this->getProperty( DAILY_GENERATION ).toObject( this->getProperty("daily_generation" ).toObject() );

    foreach(QString key , generation_data.keys() ){

        // The key can be a sorting type or directly a waste
        bool is_first_level = characterization_data.keys().contains( key );

        if( is_first_level ){ // Is sorting type

            this->generateForFullSortingType( entity , characterization_data.value( key ).toObject() , totals.value( key ) , key , generation_data.value( key ).toDouble( 0 ) );

        } else { // Is waste_category

            double total_of_totals = 0;

            foreach( QString sorting_type , characterization_data.keys() ){
                total_of_totals += characterization_data.value( sorting_type ).toObject().value( key ).toDouble( 0 );
            }

            foreach( QString sorting_type , characterization_data.keys() ){

                GWSMassUnit generate_for_this_sorting_type =
                        characterization_data.value( sorting_type ).toObject().value( key ).toDouble( 0 ) / total_of_totals * generation_data.value( key ).toDouble( 0 );

                this->generateForSortingTypeWasteCaregory( entity , sorting_type , key , generate_for_this_sorting_type );
            }
        }
    }

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}


void GenerateWasteBehaviour::generateForFullSortingType(QSharedPointer<GWSEntity> entity, QJsonObject sorting_type_data, GWSMassUnit sorting_type_total, QString sorting_type, GWSMassUnit sorting_type_generate_amount){

    GWSMassUnit existing = entity->getProperty( QString("accumulated_%1").arg( sorting_type ) ).toDouble( 0 );

    foreach (QString waste_category , sorting_type_data.keys() ) {

        // Calculate the percentage per each waste_category
        GWSMassUnit waste_category_amount = sorting_type_generate_amount * sorting_type_data.value( waste_category ).toDouble( 0 ) / sorting_type_total;
        this->generateForSortingTypeWasteCaregory( entity , sorting_type , waste_category , waste_category_amount );
    }

    entity->setProperties({
          { QString("instant_%1").arg( sorting_type ) , sorting_type_generate_amount.number() },
          { QString("accumulated_%1").arg( sorting_type ), GWSObjectFactory::incrementValue( existing.number() , sorting_type_generate_amount.number() ) },
      });
}

void GenerateWasteBehaviour::generateForSortingTypeWasteCaregory(QSharedPointer<GWSEntity> entity, QString sorting_type, QString waste_category, GWSMassUnit generate_amount){

    GWSMassUnit existing = entity->getProperty( QString("accumulated_%1_%2").arg( sorting_type ).arg( waste_category ) ).toDouble( 0 );

    entity->setProperties({
          { QString("instant_%1_%2").arg( sorting_type ).arg( waste_category )  , generate_amount.number() },
          { QString("accumulated_%1_%2").arg( sorting_type ).arg( waste_category ) , GWSObjectFactory::incrementValue( existing.number() , generate_amount.number() ) },
      });

}


