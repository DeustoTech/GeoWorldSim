

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
          return 0;
    }

}
