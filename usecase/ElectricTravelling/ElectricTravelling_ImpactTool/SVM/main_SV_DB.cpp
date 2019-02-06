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

        // List of subsegments of interest:
        QStringList subsegmentList = {
                                        // DIESEL CARS
                                       "PC diesel <1,4L Euro-1",
                                       "PC diesel <1,4L Euro-2",
                                       "PC diesel <1,4L Euro-3",
                                       "PC diesel <1,4L Euro-4",
                                       "PC diesel <1,4L Euro-5",
                                       "PC diesel <1,4L Euro-6 DPF",
                                       "PC diesel 1,4-<2L Euro-1",
                                       "PC diesel 1,4-<2L Euro-2",
                                       "PC diesel 1,4-<2L Euro-3",
                                       "PC diesel 1,4-<2L Euro-4",
                                       "PC diesel 1,4-<2L Euro-5 DPF",
                                       "PC diesel 1,4-<2L Euro-6 DPF",
                                       "PC diesel >=2L Euro-1",
                                       "PC diesel >=2L Euro-2",
                                       "PC diesel >=2L Euro-3",
                                       "PC diesel >=2L Euro-4",
                                       "PC diesel >=2L Euro-5 DPF",
                                       "PC diesel >=2L Euro-6 DPF",
                                       "PC diesel >=2L Euro-6d1 DPF",
                                        // PETROL CARS
                                        "PC petrol 1,4-<2L ECE-15-04",
                                        "PC petrol 1,4-<2L Euro-1",
                                        "PC petrol 1,4-<2L Euro-2",
                                        "PC petrol 1,4-<2L Euro-3",
                                        "PC petrol 1,4-<2L Euro-4",
                                        "PC petrol 1,4-<2L Euro-5",
                                        "PC petrol 1,4-<2L Euro-6",
                                        "PC petrol 1,4-<2L Euro-6c",
                                        "PC petrol <1,4L ECE-15-04",
                                        "PC petrol <1,4L Euro-1",
                                        "PC petrol <1,4L Euro-2",
                                        "PC petrol <1,4L Euro-3",
                                        "PC petrol <1,4L Euro-4",
                                        "PC petrol <1,4L Euro-5",
                                        "PC petrol <1,4L Euro-6",
                                        "PC petrol <1,4L Euro-6c",
                                        "PC petrol >=2L ECE-15-04",
                                        "PC petrol >=2L Euro-1",
                                        "PC petrol >=2L Euro-2",
                                        "PC petrol >=2L Euro-3",
                                        "PC petrol >=2L Euro-4",
                                        "PC petrol >=2L Euro-5",
                                        "PC petrol >=2L Euro-6",
                                        "PC petrol >=2L Euro-6c",
                                        // MOTORBIKES
                                        "MC 2S <=150cc Euro-1",
                                        "MC 2S <=150cc Euro-2",
                                        "MC 2S <=150cc Euro-3",
                                        "MC 2S <=150cc Euro-4",
                                        "MC 4S 151-250cc Euro-1",
                                        "MC 4S 151-250cc Euro-2",
                                        "MC 4S 151-250cc Euro-3",
                                        "MC 4S 151-250cc Euro-4",
                                        "MC 4S 251-750cc Euro-1",
                                        "MC 4S 251-750cc Euro-2",
                                        "MC 4S 251-750cc Euro-3",
                                        "MC 4S 251-750cc Euro-4"
                                        } ;


        // SQL query to extract the distinct pollutant components:
        QStringList componentList;
        QSqlQuery query( "SELECT DISTINCT component FROM hbefa_all" );

        while ( query.next() ){
            componentList.append( query.value(0).toString() );
        }

        // Iterate over each subsegment and component to train the SVM:
        foreach(QString subsegment , subsegmentList){

            for ( int j = 0; j < componentList.size() ; j++){

                component = componentList.at(j);

                qDebug() << subsegment << component;

                QSqlQuery query(  QString( "SELECT gradient, velocity, \"roadType\" , \"trafficSit\" , \"EFA\" FROM hbefa_all WHERE subsegment = '%1'  AND component = '%2' " ).arg( subsegment ).arg( component ) );

                QSqlError error = query.lastError();

                if( !query.isActive() ){
                    qWarning() << "ERROR: " << query.lastError().text();
                }

                QJsonArray input_list;
                QJsonArray output_list;
                QJsonObject input;
                QJsonObject output;


                if ( error.type() == QSqlError::NoError ) {
                    while( query.next() )
                    {

                        int gradient = query.value(0).toInt();
                        double velocity = query.value(1).toDouble();
                        QString roadType = query.value(2).toString();
                        double trafficSit = query.value(3).toDouble();
                        double efa = query.value(4).toDouble();

                        input.insert( "gradient" , gradient);
                        input.insert( "velocity" , velocity);
                        input.insert( "roadType" , roadType);
                        input.insert( "trafficSit" , trafficSit);
                        input_list.append( input);

                        output.insert( "EFA", efa );
                        output_list.append( output );
                    }
                }
                else {
                    qDebug() << error.text();
                }

                QString replaced = subsegment;
                QString path =  "/home/maialen/Escritorio/WorkSpace/MachineLearning/LIBSVMExamples/HBEFA/" + replaced.replace( ' ' , '_') + "/" + component ;

                QDir dir( path );
                if ( !dir.exists() ){

                    dir.mkpath(".");

                    qDebug() << "SVM" << input_list.size() << output_list.size();
                    GWSSvm* svm = new GWSSvm();
                    svm->trainFromJSON( input_list, output_list );
                    svm->saveTrained( path +  "/svm_model" , path + "/params_model" );
                    svm->deleteLater();
                }


            }

        }



        /*  QMap<QString , QVariant> test;
                    test.insert("gradient" , 0);
                    test.insert("velocity" , 98.7671890259);
                    test.insert( "roadType" , "motorway");
                    test.insert("trafficSit" , 0.66);
                   // test.insert( "subsegment", "MC 4S 251-750cc Euro-3" );
                    test.insert( "component" , "HC" );
                    svm->run( test );*/

        // Close DataBase:
        db.close();
        }


        qDebug() << "FINISHING";

        return 0;

}
