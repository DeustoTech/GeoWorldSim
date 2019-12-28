#include "ConfusionMatrix.h"

ConfusionMatrix::ConfusionMatrix(QObject *parent) : QObject(parent){

}

ConfusionMatrix::ConfusionMatrix(const ConfusionMatrix &other) : QObject(){
    this->matrix = other.matrix;
}

ConfusionMatrix::~ConfusionMatrix(){
}

/**********************************************************************
   GETTERS
**********************************************************************/

double ConfusionMatrix::getValue(QString x, QString y) const{
    return this->matrix[x][y];
}

double ConfusionMatrix::getSum(QString x) const{
    double v = 0;
    foreach(QString y , this->matrix.value( x ).keys() ){
        v += this->getValue(x , y);
    }
    return v;
}

QMap<QString , double> ConfusionMatrix::getDiagonal() const{
    QMap<QString , double> diagonal;
    foreach( QString k , this->getXNames() ){
        diagonal.insert( k , this->getValue(k , k) );
    }
    return diagonal;
}

QStringList ConfusionMatrix::getXNames() const{
    return this->matrix.keys();
}

QStringList ConfusionMatrix::getYNames(QString x) const{
    return this->matrix.value( x ).keys();
}

/**********************************************************************
   SETTERS
**********************************************************************/

void ConfusionMatrix::sumValue(QString x, QString y, double v){

    if( x.isEmpty() || y.isEmpty() ){ return; }
    if( !this->matrix.keys().contains( x ) ){
        QMap< QString , double > map;
        this->matrix.insert( x  , map );
    }
    this->matrix[x].insert( y , this->matrix.value(x).value( y , 0 ) + v );
}
