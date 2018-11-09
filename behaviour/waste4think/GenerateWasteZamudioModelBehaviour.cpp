#include "GenerateWasteZamudioModelBehaviour.h"

#include <QJsonDocument>

#include "../../app/App.h"

QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE1 = "waste_type1";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE2 = "waste_type2";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE3 = "waste_type3";
QString GenerateWasteZamudioModelBehaviour::WASTE_TYPE4 = "waste_type4";

QString GenerateWasteZamudioModelBehaviour::FAMILY_MEMBERS = "family_members";
QString GenerateWasteZamudioModelBehaviour::MAX_VALUE = "max_waste";
QString GenerateWasteZamudioModelBehaviour::NEXTS_IF_TRUE = "nexts_if_true";
QString GenerateWasteZamudioModelBehaviour::NEXTS_IF_FALSE = "nexts_if_false";
QString GenerateWasteZamudioModelBehaviour::STORE_GENERATED_WASTE_AS = "store_generated_waste_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE1_AS = "store_waste_type1_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE2_AS = "store_waste_type2_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE3_AS = "store_waste_type3_as";
QString GenerateWasteZamudioModelBehaviour::STORE_WASTE_TYPE4_AS = "store_waste_type4_as";

GenerateWasteZamudioModelBehaviour::GenerateWasteZamudioModelBehaviour() : GWSBehaviour(){

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



    this->setProperty( "characterization" , characterization );
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



QStringList GenerateWasteZamudioModelBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString charac_str = this->getProperty( "characterization" ).toString();
    //qDebug() << charac_str;
    QJsonDocument jsonCharac = QJsonDocument::fromJson( charac_str.toUtf8() );
    //qDebug() << jsonCharac;
    QJsonObject characObject = jsonCharac.object();
    //qDebug() << characObject;
    QJsonValue restoObject = characObject["resto"];
    //qDebug() << restoObject;

    QString waste_type1 = this->getProperty( WASTE_TYPE1 ).toString();
    //QString waste_type2 = this->getProperty( WASTE_TYPE2 ).toString();
    //QString waste_type3 = this->getProperty( WASTE_TYPE3 ).toString();
    //QString waste_type4 = this->getProperty( WASTE_TYPE4 ).toString();

    double despilfarro_alimento = restoObject[ waste_type1 ].toDouble();
    //double no_despilfarro_alimento = restoObject[ waste_type2 ].toDouble();
    //double jardineria = restoObject[ waste_type3 ].toDouble();
    //double carton_envase = restoObject[ waste_type4 ].toDouble();
    //qDebug() << no_despilfarro_alimento;
    qDebug() << despilfarro_alimento;
    //qDebug() << jardineria;
    //qDebug() << carton_envase;


    // charac_string = this->getProperty( "characterization" );

    //agent->setProperty( "characterization" , characterization );
    //agent->setProperty( this->getProperty( STORE_GENERATED_WASTE_AS ).toString() , daily_waste_amount  );

    /*QVariant max_value = this->getProperty( MAX_VALUE);

    // Keep incrementing
    if ( agent_waste_amount < max_value ){

        QVariant waste_incremented = agent_waste_amount.toDouble() + this->getProperty( FAMILY_MEMBERS ).toDouble();
        if( max_value.isValid() ){ waste_incremented = qMin( max_value , waste_incremented ); }

        // agent->setProperty( waste_type , waste_incremented );
        agent->setProperty( STORE_GENERATED_WASTE_AS , waste_incremented );
        QStringList next = this->getProperty( NEXTS_IF_FALSE ).toStringList();

        emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
        return next;
    }
    // Your are full, go to next behaviour
    if ( agent_waste_amount >= max_value  ){
        QStringList next = this->getProperty( NEXTS_IF_TRUE ).toStringList();
        return next;
    }*/

    // Set waste generation by means of partial model:
    // 2016 data:
    double rest = 9.56 ;
    double uni = 20.16 ;
    double paro =  7.65;
    double urb = 12.48 ;
    double waste = GenerateWasteZamudioModelBehaviour::partialModel( rest , uni , paro , urb ) ;

    agent->setProperty( this->getProperty( STORE_WASTE_TYPE1_AS ).toString() , despilfarro_alimento  );

    //agent->setProperty( this->getProperty( STORE_GENERATED_WASTE_AS ).toString() , waste / agent->getProperty( "TOTAL" ).toDouble() );

    emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );

    QStringList nexts = this->getProperty( NEXTS_IF_TRUE ).toStringList();
    return nexts;

}


