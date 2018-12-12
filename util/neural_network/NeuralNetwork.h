#ifndef GWSNEURALNETWORK_H
#define GWSNEURALNETWORK_H

#include <QObject>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>

#include "doublefann.h"
#include "fann_cpp.h"
#include "fann_train.h"
#include "fann_data.h"

class GWSNeuralNetwork : public QObject{
    Q_OBJECT

public:
    explicit GWSNeuralNetwork(int learning_rate, int num_layers, int num_hidden, double desired_error, int max_iterations, int iterations_between_reports, QObject *parent = 0);
    explicit GWSNeuralNetwork(QString training_path);

    // METHODS
    void trainFromFile( QString inputs_file_path , QString outputs_file_path );
    void train( QJsonArray inputs_array , QJsonArray outputs_array  );
    QJsonObject run( QJsonObject inputs );

private:

    // NN Parameters
    float learning_rate;

    unsigned int num_layers;
    unsigned int num_hidden;

    float desired_error;
    unsigned int max_iterations;
    unsigned int iterations_between_reports;

    FANN::training_data train_data;
    FANN::neural_net net;

    // Inputs
    QMap<QString , int> input_positions; // { "input1:value_str1" : 0 , "input1:value_str2" : 1 , "input2" : 3 }

    // Outputs
    QMap<QString , int> output_positions; // { "output1:value_str1" : 0 , "output1:value_str2" : 1 , "output2" : 3 }

    QMap<QString , int> generatePositions( QJsonArray ios );

};

#endif // GWSNEURALNETWORK_H
