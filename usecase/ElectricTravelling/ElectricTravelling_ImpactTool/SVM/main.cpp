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


// Utils
#include "../../../../util/ai/Intelligence.h"
#include "../../../../util/svm/Svm.h"


int main(int argc, char **argv)
{



    // Check SVM input and output files are parsed as arguments
    /*if (argc < 2) {
          qDebug() << "Input or output files missing! Cannot execute. ";
          return 0;
    } else {*/

    // Loop over all vehicle subtype directories and components
    QString svm_splitted_file_path = "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/HBFA_SVM_SPLITTED/";
    QDir directory( svm_splitted_file_path );

    // Vehicle subtype level:
    QFileInfoList vehicleSubsegmentsInfo = directory.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QFileInfo subsegment, vehicleSubsegmentsInfo) {

        if ( subsegment.isDir() ){

            QFileInfoList pollutionComponentsInfo = QDir( subsegment.absoluteFilePath() ).entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs );

            foreach( QFileInfo component, pollutionComponentsInfo) {

                if ( component.isDir() ){

                    QFileInfoList pollutantFilesInfo = QDir( component.absoluteFilePath() ).entryInfoList( QDir::NoDotAndDotDot | QDir::Files);

                    QString inputs_file_path;
                    QString outputs_file_path;

                    foreach( QFileInfo pollutantFiles, pollutantFilesInfo) {

                        if ( pollutantFiles.isFile() ){
                            qDebug() << pollutantFiles.absoluteFilePath();
                            QString fileName = pollutantFiles.fileName();

                            if ( fileName == "INPUT.csv"  ){
                                qDebug() << "FOUND INPUTS FILE!";
                                inputs_file_path = pollutantFiles.absoluteFilePath();
                            }

                            if ( fileName == "OUTPUT.csv"  ){
                                qDebug() << "FOUND OUTPUTS FILE!";
                                outputs_file_path = pollutantFiles.absoluteFilePath();
                            }

                        }

                    }

                    GWSSvm* svm = new GWSSvm();
                    svm->trainFromFile( inputs_file_path , outputs_file_path );
                    svm->saveModel( component.absoluteFilePath() + "/out" );
                    svm->saveTrained( component.absoluteFilePath() + "/test_out" , component.absoluteFilePath() + "/test_params");
                    svm->deleteLater();

                }
            }
        }

    }




         // svm->trainFromFile( inputs_file_path, outputs_file_path);
         // svm->saveTrained( "/home/maialen/test_svm" , "/home/maialen/test_parameters");



          //QList<QPair < QString, QVariant> > test_input;
         /* test_input = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/St+Go"),
                         QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S 251-750cc Euro-5") ,
                         QPair<QString , QVariant>("V" , 28.1581172943) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};
            */


         // 0;98.7671890259;RUR/Semi-MW/110/Freeflow
         /* QMap<QString , QVariant> input;
          input.insert("Gradient" , 0);
          input.insert("V" , 98.7671890259);
          input.insert("TrafficSit" , "RUR/Semi-MW/110/Freeflow");
          qDebug() << svm->run( input );
          return 0;
    //}*/

}
