#include "NumberSorter.h"

NumberSorter::NumberSorter(QObject *parent) : QObject(parent){

}

QVector<double> NumberSorter::sortAscending(QVector<double> unsorted_list){

    QVector<double> sorted;

    while( !unsorted_list.isEmpty() ){
        double min_index = 0;
        for( int i = 0 ; i < unsorted_list.size() ; i++) {
            if( unsorted_list.at( i ) < unsorted_list.at( min_index ) ){
                min_index = i;
            }
        }

        sorted.append( unsorted_list.takeAt( min_index ) );
    }

    return sorted;
}
