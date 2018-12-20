

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


          //QList<QPair < QString, QVariant> > test_input;
         /* test_input = { QPair<QString , QVariant>("Component" , "HC") , QPair<QString , QVariant>("TrafficSit" , "RUR/MW/80/St+Go"),
                         QPair<QString , QVariant>("Gradient" , 0) , QPair<QString , QVariant>("Subsegment" , "MC 4S 251-750cc Euro-5") ,
                         QPair<QString , QVariant>("V" , 28.1581172943) , QPair<QString , QVariant>("V_0" , 0) , QPair<QString , QVariant>("V_100" , 0)};
            */


          // HC	RUR/MW/80/Freeflow	0	MC 4S >750cc Euro-5	90,022644043	0	0

          QMap<QString , QVariant> input;
          input.insert("Component" , "HC");
          input.insert("TrafficSit" , "RUR/MW/80/Freeflow");
          input.insert( "Gradient" , 0);
          input.insert("Subsegment" , "MC 4S >750cc Euro-5");
          input.insert("V" , 90.022644043);
          input.insert("V_0" , 0);
          input.insert("V_100" , 0);
          svm->run( input );
          return 0;
    }

}
