#ifndef GWSNEURALNETWORK_H
#define GWSNEURALNETWORK_H

#include <QObject>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>

#include "fann.h"
#include "fann_cpp.h"
#include "fann_train.h"
#include "fann_data.h"

class GWSNeuralNetwork : public QObject{
    Q_OBJECT

public:
    explicit GWSNeuralNetwork(double learning_rate,  double desired_error, int max_iterations, int iterations_between_reports );
    explicit GWSNeuralNetwork(QString training_path);
    ~GWSNeuralNetwork();

    // METHODS
    void trainFromFile( QString inputs_file_path , QString outputs_file_path );
    void trainFromJSON( QJsonArray inputs_array , QJsonArray outputs_array  );
    void train( QList< QList< QPair< QString , QVariant> > > input_train_dataset, QList< QList< QPair< QString , QVariant> > >  output_train_dataset );
    void save( const std::string trained_network_filename );
    QJsonObject run( QJsonObject inputs );

private:

    // NN Parameters
    float learning_rate;
    unsigned int num_hidden;

    float desired_error;
    unsigned int max_iterations;
    unsigned int iterations_between_reports;

    FANN::training_data train_data;
    FANN::neural_net net;

    // Inputs
    QMap<QString , int>* input_positions = Q_NULLPTR; // { "input1:value_str1" : 0 , "input1:value_str2" : 1 , "input2" : 3 }
    QMap< QString , double >* input_maximums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double >* input_minimums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 0 ...

    // Outputs
    QMap<QString , int>* output_positions = Q_NULLPTR;; // { "output1:value_str1" : 0 , "output1:value_str2" : 1 , "output2" : 3 }
    QMap< QString , double >* output_maximums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 42 ...
    QMap< QString , double >* output_minimums = Q_NULLPTR;; // { pos0 : 2.43 , pos1 : 0 ...

    void generatePositions( QList< QList< QPair< QString , QVariant> > > data_rows ,  QMap<QString , int>* positions , QMap< QString , double >* maximums , QMap< QString , double >* minimums );

};

#endif // GWSNEURALNETWORK_H
