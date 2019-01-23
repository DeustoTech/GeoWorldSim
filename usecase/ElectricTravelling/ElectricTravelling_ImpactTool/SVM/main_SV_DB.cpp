#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

// Utils
#include "../../../../util/ai/Intelligence.h"
#include "../../../../util/svm/Svm.h"


int main(int argc, char **argv)
{

    const QString DRIVER("QPSQL");
    if( QSqlDatabase::isDriverAvailable( DRIVER) ){

        QSqlDatabase db = QSqlDatabase::addDatabase( DRIVER);
        db.setHostName( "localhost" );
        db.setDatabaseName( "hbefa" );
        db.setUserName( "maialen" );


        if( !db.open() )
          qWarning() << "ERROR: " << db.lastError();

        QString component;
        QString subsegment;

        QJsonArray input_list;
        QJsonArray output_list;
        QJsonObject input;
        QJsonObject output;

        QSqlQuery query( "SELECT gradient, velocity, \"roadType\" , component, subsegment, \"trafficSit\" , \"EFA\" FROM hbefa_all WHERE subsegment = 'MC 4S 251-750cc Euro-3' AND component = 'HC' " );
        QSqlError error = query.lastError();

        if( !query.isActive() ){
            qWarning() << "ERROR: " << query.lastError().text();}

        if ( error.type() == QSqlError::NoError ) {
            int i = 0;


            while( query.next() )
            {

                int gradient = query.value(0).toInt();
                double velocity = query.value(1).toDouble();
                QString roadType = query.value(2).toString();
                component = query.value(3).toString();
                subsegment = query.value(4).toString();
                double trafficSit = query.value(5).toDouble();
                double efa = query.value(6).toDouble();

                input.insert( "gradient" , gradient);
                input.insert( "velocity" , velocity);
                input.insert( "subsegment", subsegment );
                input.insert( "roadType" , roadType);
                input.insert( "trafficSit" , trafficSit);
                input.insert( "component" , component );

                input_list.append( input);

                output.insert( "EFA", efa );
                output_list.append( output );

                i++;
            }
        }
        else {
            qDebug() << error.text();
        }



        QString path =  "/home/maialen/Escritorio/WorkSpace/MachineLearning/LIBSVMExamples/HBEFA/" + subsegment.replace( ' ' , '_') + "/" + component ;

        QDir dir( path );
        if (!dir.exists()){
            dir.mkpath(".");
            GWSSvm* svm = new GWSSvm();
            svm->trainFromJSON( input_list, output_list);
            svm->saveTrained( path +  "/svm_model" , path + "/params_model" );
            svm->deleteLater();
        }


        else {

            GWSSvm* svm = new GWSSvm();

            svm->loadTrained( path +  "/svm_model" , path + "/params_model" );

            QMap<QString , QVariant> test;
                      test.insert("gradient" , 0);
                      test.insert("velocity" , 98.7671890259);
                      test.insert( "roadType" , "motorway");
                      test.insert("trafficSit" , 0.66);
                      test.insert( "subsegment", "MC 4S 251-750cc Euro-3" );
                      test.insert( "component" , "HC" );
                      svm->run( test );

        }




        // Close DataBase:
        db.close();
        }


              return 0;



}
