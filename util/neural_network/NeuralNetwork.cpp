#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(int learning_rate, int num_layers, int num_input, int num_hidden, int num_output, double desired_error, int max_iterations, int iterations_between_reports, QObject *parent) : QObject(parent){
    this->learning_rate = learning_rate;
    this->num_layers = num_layers;
    this->num_input = num_input;
    this->num_hidden = num_hidden;
    this->num_output = num_output;
    this->desired_error = desired_error;
    this->max_iterations = max_iterations;
    this->iterations_between_reports = iterations_between_reports;

    this->net.create_standard(num_layers, num_input, num_hidden, num_output);
    this->net.set_learning_rate(learning_rate);

    this->net.set_activation_steepness_hidden(1.0);
    this->net.set_activation_steepness_output(1.0);

    this->net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    this->net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);
}

NeuralNetwork::NeuralNetwork( QString training_file ){
    FANN::training_data data;

}

/**********************************************************************
  METHODS
**********************************************************************/

void NeuralNetwork::train(QString data_path){

    FANN::training_data data;
    data.read_train_from_file( data_path.toStdString() );

    this->net.init_weights( data );
    this->net.train_on_data(data, this->max_iterations, this->iterations_between_reports, this->desired_error);
}

double NeuralNetwork::run( QList<double>  input_values ){
    fann_type input[ input_values.size() ];
    for( int i = 0; i < input_values.size(); i++ ){
        input[i] = input_values.at( i );
    }

    fann_type* output = this->net.run( input  );
    return 0;
}
