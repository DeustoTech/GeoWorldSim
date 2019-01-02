#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>


// Utils
#include "../../../../util/ai/Intelligence.h"
#include "../../../../util/svm/Svm.h"


int main(int argc, char **argv)
{
    GWSSvm* svm = new GWSSvm();

    // Check SVM input and output files are parsed as arguments
    if (argc < 2) {
          qDebug() << "Input or output files missing! Cannot execute. ";
          return 0;
    } else {

          QString inputs_file = argv[1];
          QString outputs_file = argv[2];

          svm->trainFromFile( inputs_file, outputs_file);
          svm->saveTrained( "/home/maialen/test_svm" , "/home/maialen/test_parameters");

          //QList<QPair < QString, QVariant> > test_input;
         /* test_input = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/St+Go"),
                         QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S 251-750cc Euro-5") ,
                         QPair<QString , QVariant>("V" , 28.1581172943) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};
            */


         // 0;98.7671890259;RUR/Semi-MW/110/Freeflow
          QMap<QString , QVariant> input;
          input.insert("Gradient" , 0);
          input.insert("V" , 98.7671890259);
          input.insert("TrafficSit" , "RUR/Semi-MW/110/Freeflow");
          qDebug() << svm->run( input );
          return 0;
    }

}
