#include "Statistics.h"

#include <QtMath>

Statistics::Statistics(QObject *parent) : QObject(parent){

}


/**********************************************************************
   GETTERS
**********************************************************************/

double Statistics::mean(QList<double> values){
    double mean = values.at(0);
    foreach( double v , values ){
        mean = (mean + v) / 2;
    }
    return mean;
}

double Statistics::std(QList<double> values){
    return qSqrt( Statistics::variance( values ) );
}

double Statistics::variance(QList<double> values){
    double mean = Statistics::mean( values );
    double variance = 0;
    foreach( double v , values ){
        variance += (v-mean)*(v-mean);
    }
    return variance / values.size();
}

double Statistics::max(QList<double> values){
    double max = values.at(0);
    foreach(double v , values){
        if( v > max ){ max = v; }
    }
    return max;
}

double Statistics::min(QList<double> values){
    double min = values.at(0);
    foreach(double v , values){
        if( v < min ){ min = v; }
    }
    return min;
}
