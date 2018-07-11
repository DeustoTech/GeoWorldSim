#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QObject>

#include "fann.h"
#include "floatfann.h"
#include "fann_cpp.h"


class NeuralNetwork : public QObject{
    Q_OBJECT

public:
    explicit NeuralNetwork(int learning_rate, int num_layers, int num_input, int num_hidden, int num_output, double desired_error, int max_iterations, int iterations_between_reports, QObject *parent = 0);
    explicit NeuralNetwork(QString training_path);

    // METHODS
    void train( QString data_path );
    double run(QList<double>  input_values);

private:

    // NN Parameters
    float learning_rate;
    unsigned int num_layers;
    unsigned int num_input;
    unsigned int num_hidden;
    unsigned int num_output;
    float desired_error;
    unsigned int max_iterations;
    unsigned int iterations_between_reports;

    FANN::neural_net net;


};

#endif // NEURALNETWORK_H
