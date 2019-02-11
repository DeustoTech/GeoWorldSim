#include "GenerateWasteZamudioModelBehaviour.h"

#include <QJsonDocument>

#include "../../app/App.h"

QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE1 = "waste_type1";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE2 = "waste_type2";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE3 = "waste_type3";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE4 = "waste_type4";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE5 = "waste_type5";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE6 = "waste_type6";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE7 = "waste_type7";


QString GenerateWasteZamudioModelBehaviour::FAMILY_MEMBERS = "family_members";
QString GenerateWasteZamudioModelBehaviour::MAX_VALUE = "max_waste";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE1_AS = "store_waste_type1_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE2_AS = "store_waste_type2_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE3_AS = "store_waste_type3_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE4_AS = "store_waste_type4_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE5_AS = "store_waste_type5_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE6_AS = "store_waste_type6_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE7_AS = "store_waste_type7_as";

QString GenerateWasteZamudioModelBehaviour::NEXTS = "nexts";

GenerateWasteZamudioModelBehaviour::GenerateWasteZamudioModelBehaviour() : GWSBehaviour(){

    // Careful! These are the quantities obtained from a whole container.
    // Right now, the model assumes each household generates this HUGE amount of waste
    QString characterization = "{"
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

    QJsonDocument jsonCharac = QJsonDocument::fromJson( characterization.toUtf8() );
    QJsonObject characObject = jsonCharac.object();


    this->setProperty( "characterization" , characObject );
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


double GenerateWasteZamudioModelBehaviour::partialModel ( double rest, double uni, double tasParo , double urb){

    // Fracción resto kg / habitante / año:

    double model = 344.57 + 2.56 * rest  + 1.44 * uni - 1.42 * tasParo - 1.01 * urb;
    return model;
}


QJsonArray GenerateWasteZamudioModelBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString waste_type1 = this->getProperty( WASTE_TYPE1 ).toString(); // e.g. fraccion resto
    QString waste_type2 = this->getProperty( WASTE_TYPE2 ).toString();
    QString waste_type3 = this->getProperty( WASTE_TYPE3 ).toString();
    QString waste_type4 = this->getProperty( WASTE_TYPE4 ).toString();
    QString waste_type5 = this->getProperty( WASTE_TYPE5 ).toString();
    QString waste_type6 = this->getProperty( WASTE_TYPE6 ).toString();
    QString waste_type7 = this->getProperty( WASTE_TYPE7 ).toString();

    QJsonObject characObject = this->getProperty( "characterization" ).toObject();
    QJsonValue ST1Object = characObject[ waste_type1 ];
    QJsonValue ST2Object = characObject[ waste_type1 ];
    QJsonValue ST3Object = characObject[ waste_type1 ];
    QJsonValue ST4Object = characObject[ waste_type1 ];
    QJsonValue ST5Object = characObject[ waste_type1 ];
    QJsonValue ST6Object = characObject[ waste_type1 ];
    QJsonValue ST7Object = characObject[ waste_type1 ];

    agent->setProperty( waste_type1 , ST1Object .toObject() );
    agent->setProperty( waste_type2 , ST2Object .toObject() );
    agent->setProperty( waste_type3 , ST3Object .toObject() );   
    agent->setProperty( waste_type4 , ST3Object .toObject() );
    agent->setProperty( waste_type5 , ST3Object .toObject() );
    agent->setProperty( waste_type6 , ST3Object .toObject() );
    agent->setProperty( waste_type7 , ST3Object .toObject() );

    //qDebug() << agent->getProperty( waste_type1 );
    // Your are full, go to next behaviour
    /*QStringList nexts;
    if ( total_amount_waste1 >= this->getProperty( MAX_VALUE ).toDouble()  ){
        nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();
    }
    else {
        nexts = this->getProperty( NEXTS_IF_FALSE ).toStringList();
    }*/


    return this->getProperty( NEXTS ).toArray();

}


