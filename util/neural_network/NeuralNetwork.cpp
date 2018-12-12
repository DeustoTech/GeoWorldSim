#include "NeuralNetwork.h"

#include <QJsonObject>
#include <QDebug>

GWSNeuralNetwork::GWSNeuralNetwork(int learning_rate, int num_layers, int num_hidden, double desired_error, int max_iterations, int iterations_between_reports, QObject *parent) : QObject(parent){

    this->learning_rate = learning_rate;

    this->num_layers = num_layers;
    this->num_hidden = num_hidden;

    this->desired_error = desired_error;
    this->max_iterations = max_iterations;
    this->iterations_between_reports = iterations_between_reports;
}

GWSNeuralNetwork::GWSNeuralNetwork( QString training_file ){
    FANN::training_data data;

}

/**********************************************************************
  METHODS
**********************************************************************/

void GWSNeuralNetwork::trainFromFile(QString inputs_file_path, QString outputs_file_path) {

    //FANN::training_data data;
    //data.read_train_from_file( data_path.toStdString() );

    //this->net.init_weights( data );
    //this->net.train_on_data(data, this->max_iterations, this->iterations_between_reports, this->desired_error);
}

void GWSNeuralNetwork::train( QJsonArray input_train_dataset , QJsonArray output_train_dataset ){

    Q_ASSERT( input_train_dataset.size() == output_train_dataset.size() );

    this->input_positions = this->generatePositions( input_train_dataset );
    this->output_positions = this->generatePositions( output_train_dataset );

    // Initialize input set:
    double** fann_inputs = new double* [ input_train_dataset.size() ]; // Row amount of the dataset

    for ( unsigned int i = 0 ; i < input_train_dataset.size() ; i++ ){ // Columns tha represent the inputs of the dataset

        fann_inputs[i] = new double[ this->input_positions.size() ];

        // Init all positions to 0
        for(int j = 0 ; j < this->input_positions.size() ; j++ ){
            fann_inputs[i][j] = 0;
        }

        // Go row by row setting the inputs
        QJsonObject row = input_train_dataset.at( i ).toObject();
        foreach (QString input_name , row.keys()) {

            QString hash = input_name;
            if( row.value( input_name ).isString() ){
                hash = input_name + ":" + row.value( input_name ).toString();
            }

            int position = this->input_positions.value( hash , -1 );
            if( position > -1 ){
                fann_inputs[i][position] = row.value( input_name ).toDouble( 1 );
            }

        }
    }

    // Initialize and fill output set:
    double ** fann_outputs = new double* [ output_train_dataset.size() ];

    for ( unsigned int i = 0 ; i < output_train_dataset.size() ; i++ ){  // Columns tha represent the outputs of the dataset

        fann_outputs[i] = new double[ this->output_positions.size() ];

        // Init all positions to 0
        for(int j = 0 ; j < this->output_positions.size() ; j++ ){
            fann_outputs[i][j] = 0;
        }

        // Go row by row setting the inputs
        QJsonObject row = output_train_dataset.at( i ).toObject();
        foreach (QString output_name , row.keys()) {

            QString hash = output_name;
            if( row.value( output_name ).isString() ){
                hash = output_name + ":" + row.value( output_name ).toString();
            }

            int position = this->output_positions.value( hash , -1 );
            if( position > -1 ){
                fann_outputs[i][position] = row.value( output_name ).toDouble( 1 );
            }

        }

    }

    // Divide data into training and testing batches:
    this->train_data.set_train_data( input_train_dataset.size() , this->input_positions.size() , fann_inputs , this->output_positions.size() , fann_outputs );

    this->learning_rate = 0.7;
    this->num_layers = 3;
    this->num_hidden = 13;
    this->desired_error = 0.001;
    this->max_iterations = 300000;
    this->iterations_between_reports = 1000;

    qDebug() << "Creating network." << endl;

    this->net.create_standard( this->num_layers, this->input_positions.size(), this->num_hidden, this->output_positions.size());
    this->net.set_learning_rate( this->learning_rate );

    this->net.set_activation_steepness_hidden(1.0);
    this->net.set_activation_steepness_output(1.0);

    this->net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    this->net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

    // Set additional properties such as the training algorithm
    //net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

    // Print NN parameters
    //this->net.print_parameters();

    qDebug() << endl << "Training network." << endl;

    // Train the network with the training dataset:
    this->net.init_weights( this->train_data );
    this->net.train_on_data( this->train_data , this->max_iterations , this->iterations_between_reports , this->desired_error );

    // Save trained network:
    this->net.save( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/Waste4Think_V1/trained_network.net");

}

QJsonObject GWSNeuralNetwork::run( QJsonObject inputs ){

    fann_type input[ this->input_positions.size() ];

    // Init all positions to 0
    for(int j = 0 ; j < this->input_positions.size() ; j++ ){
        input[j] = 0;
    }

    // Iterate received inputs
    foreach( QString input_name , inputs.keys() ) {

        QString hash = input_name;
        if( inputs.value( input_name ).isString() ){
            hash = input_name + ":" + inputs.value( input_name ).toString();
        }

        int position = this->input_positions.value( hash , -1 );
        if( position > -1 ){
            input[position] = inputs.value( input_name ).toDouble( 1 );
        }
    }

    fann_type* output = this->net.run( input );
    return QJsonObject();
}


/**********************************************************************
  PRIVATE
**********************************************************************/

QMap<QString , int> GWSNeuralNetwork::generatePositions(QJsonArray ios){

    QMap<QString , int> positions;
    int next_position = 0;

    // Give positions to the outputs
    foreach(QJsonValue row, ios) {

        foreach (QString io_name , row.toObject().keys() ) {

            QString hash = io_name;
            QJsonValue io_value = row.toObject().value( io_name );

            if( io_value.isString() ){
                hash = io_name + ":" + io_value.toString();
            }

            if( !positions.keys().contains( hash ) ){
                positions.insert( hash , next_position );
                next_position++;
            }
        }

    }

    return positions;
}
