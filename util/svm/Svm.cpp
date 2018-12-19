#include "Svm.h"

#include <QDebug>

GWSSvm::GWSSvm() : GWSIntelligence (){
}

GWSSvm::~GWSSvm(){
}


/**********************************************************************
  METHODS
**********************************************************************/

/* NOTE [from https://www.csie.ntu.edu.tw/~cjlin/libsvm/faq.html#/Q03:_Data_preparation]:
 * Currently libsvm supports only numerical data. You may have to change non-numerical data to numerical.
 * For example, you can use several binary attributes to represent a categorical attribute.
 */


/* Train SVM on given files */

void GWSSvm::train(QList<QList<QPair<QString, QVariant> > > input_train_dataset, QList<QList<QPair<QString, QVariant> > > output_train_dataset){




}


