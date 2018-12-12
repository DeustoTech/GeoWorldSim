#ifndef NEURALNETWORKFROMDATASOURCE_H
#define NEURALNETWORKFROMDATASOURCE_H

#include "doublefann.h"
#include "fann_cpp.h"
#include "fann_train.h"
#include "fann_data.h"
#include "fann_internal.h"
#include "fann_io.h"
#include "fann_error.h"
#include <QObject>


class NeuralNetworkFromDatasource : public QObject
{

    Q_OBJECT

public:
        explicit  NeuralNetworkFromDatasource( QString url );


public slots:
        // METHODS
        void generateDataset( int train_pos , int train_length , int test_pos , int test_length );
        //void createNetwork();
        //void train( );
        void test();
        void run( double* input_values );

private:

        // NN Parameters
        QString url;
        double learning_rate;
        unsigned int num_data;
        unsigned int num_layers;
        unsigned int num_input;
        unsigned int num_hidden;
        unsigned int num_output;
        double desired_error;
        unsigned int max_iterations;
        unsigned int iterations_between_reports;

        FANN::neural_net net;

        double** input_set;
        double** output_set;

        FANN::training_data train_data;
        FANN::training_data test_data;

        double* output;


};


#endif // NEURALNETWORKFROMDATASOURCE_H
