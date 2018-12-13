#include "NeuralNetwork.h"

#include <QJsonObject>
#include <QDebug>

GWSNeuralNetwork::GWSNeuralNetwork(double learning_rate, double desired_error, int max_iterations, int iterations_between_reports ) : QObject( ){

    this->learning_rate = learning_rate;

    this->num_hidden = num_hidden;

    this->desired_error = desired_error;
    this->max_iterations = max_iterations;
    this->iterations_between_reports = iterations_between_reports;

    this->input_positions = new QMap<QString , int>();
    this->output_positions = new QMap<QString , int>();

    this->input_maximums = new QMap<QString , double>();
    this->input_minimums = new QMap<QString , double>();
    this->output_maximums = new QMap<QString , double>();
    this->output_minimums = new QMap<QString , double>();
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

    this->generatePositions( input_train_dataset  , this->input_positions , this->input_maximums , this->input_minimums );
    this->generatePositions( output_train_dataset , this->output_positions , this->output_maximums , this->output_minimums );

    // Initialize input set:
    fann_type** fann_inputs = new fann_type* [ input_train_dataset.size() ]; // Row amount of the dataset

    for ( int i = 0 ; i < input_train_dataset.size() ; i++ ){ // Columns tha represent the inputs of the dataset

        fann_inputs[i] = new fann_type[ this->input_positions->keys().size() ];

        // Init all positions to 0
        for(int j = 0 ; j < this->input_positions->keys().size() ; j++ ){
            fann_inputs[i][j] = 0;
        }

        // Go row by row setting the inputs
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QJsonObject row = input_train_dataset.at( i ).toObject();
        foreach (QString input_name , row.keys()) {

            QString hash = input_name;
            if( row.value( input_name ).isString() ){
                hash = input_name + ":" + row.value( input_name ).toString();
            }
            if( row.value( input_name ).isDouble() ){
                double min = this->input_minimums->value( input_name );
                double max = this->input_maximums->value( input_name );
                row[ input_name ] = ( row.value( input_name ).toDouble() - min ) / ( max - min );
            }

            int position = this->input_positions->value( hash , -1 );
            if( position > -1 ){
                fann_inputs[i][position] = row.value( input_name ).toDouble( 1 );
            }

        }
    }

    // Initialize and fill output set:
    fann_type** fann_outputs = new fann_type* [ output_train_dataset.size() ];

    for ( int i = 0 ; i < output_train_dataset.size() ; i++ ){  // Columns tha represent the outputs of the dataset

        fann_outputs[i] = new fann_type[ this->output_positions->keys().size() ];

        // Init all positions to 0
        for(int j = 0 ; j < this->output_positions->keys().size() ; j++ ){
            fann_outputs[i][j] = 0;
        }

        // Go row by row setting the inputs and scale if neccesary:
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QJsonObject row = output_train_dataset.at( i ).toObject();
        foreach (QString output_name , row.keys()) {

            QString hash = output_name;
            if( row.value( output_name ).isString() ){
                hash = output_name + ":" + row.value( output_name ).toString();
            }
            if( row.value( output_name ).isDouble() ){
                double min = this->output_minimums->value( output_name );
                double max = this->output_maximums->value( output_name );
                row[ output_name ] = ( row.value( output_name ).toDouble() - min ) / ( max - min );
            }

            int position = this->output_positions->value( hash , -1 );
            if( position > -1 ){
                fann_outputs[i][position] = row.value( output_name ).toDouble( 1 );
            }

        }

    }

    // Divide data into training and testing batches:
    this->train_data.set_train_data( input_train_dataset.size() , this->input_positions->keys().size() , fann_inputs , this->output_positions->keys().size() , fann_outputs );

    qDebug() << "Creating network." << endl;

    // this->net.create_standard( this->input_positions->size() + this->output_positions->size(), this->input_positions->size(), this->num_hidden, this->output_positions->size());

    // Crash when using create_standard method if total layers > 3. Using create_standard_array instead:
    unsigned int layers[6] = { (unsigned int)this->input_positions->keys().size()  , 8 , 9 ,  (unsigned int)this->output_positions->keys().size() };
    this->net.create_standard_array(4, layers);

    this->net.set_learning_rate( this->learning_rate );
    this->net.set_activation_steepness_hidden(1.0);
    this->net.set_activation_steepness_output(1.0);
    this->net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    this->net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

    // Set additional properties such as the training algorithm
    //net.set_training_algorithm(FANN::TRAIN_INCREMENTAL);

    // Print NN parameters
    this->net.print_parameters();

    qDebug() << endl << "Training network." << endl;

    // Train the network with the training dataset:
    this->net.init_weights( this->train_data );
    this->net.train_on_data( this->train_data , this->max_iterations , this->iterations_between_reports , this->desired_error );

    // Save trained network:
    this->net.save( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/Waste4Think_V1/trained_network.net");

}

QJsonObject GWSNeuralNetwork::run( QJsonObject inputs ){

    fann_type input[ this->input_positions->keys().size() ];

    // Init all positions to 0
    for(int j = 0 ; j < this->input_positions->keys().size() ; j++ ){
        input[j] = 0;
    }

    // Iterate received inputs
    double min;
    double max;
    foreach( QString input_name , inputs.keys() ) {

        QString hash = input_name;
        if( inputs.value( input_name ).isString() ){
            hash = input_name + ":" + inputs.value( input_name ).toString();
        }
        if( inputs.value( input_name ).isDouble() ){
            min = this->input_minimums->value( input_name );
            max = this->input_maximums->value( input_name );
            inputs[ input_name ] = ( inputs.value( input_name ).toDouble() - min ) / ( max - min );
        }
        int position = this->input_positions->value( hash , -1 );
        if( position > -1 ){
            input[position] = inputs.value( input_name ).toDouble( 1 );
        }
    }

    fann_type* output = this->net.run( input );

    foreach( int position , this->output_positions->values() ){
        double normalized_output = output[position];
        qDebug() << "Normalized output for" << this->output_positions->key( position ) << " = " << normalized_output;
        double output_min = this->output_minimums->value( this->output_positions->key( position ) );
        double output_max = this->output_maximums->value( this->output_positions->key( position ) );
        double denormalized_output = normalized_output * ( output_max - output_min) + output_min;
        qDebug() << "Denormalized output for " << this->output_positions->key( position ) << " = " <<  denormalized_output;
    }

    return QJsonObject();

}


/**********************************************************************
  PRIVATE
**********************************************************************/

void GWSNeuralNetwork::generatePositions(QJsonArray ios, QMap<QString, int> *positions, QMap<QString, double> *maximums, QMap<QString, double> *minimums){

    int next_position = 0;

    // Give positions to the outputs
    foreach(QJsonValue row, ios) {

        foreach (QString io_name , row.toObject().keys() ) {

            QString hash = io_name;
            QJsonValue io_value = row.toObject().value( io_name );

            if( io_value.isString() ){
                hash = io_name + ":" + io_value.toString();
            }

            // Initialize position
            if( !positions->keys().contains( hash ) ){
                positions->insert( hash , next_position );
                next_position++;
            }

            // Initialize maximum
            if( !maximums->keys().contains( hash ) ){
                maximums->insert( hash , -9999999999999 );
            }
            maximums->insert( hash , qMax( maximums->value( hash ) , io_value.toDouble( 1 ) ) );

            // Initialize minimum
            if( !minimums->keys().contains( hash ) ){
                minimums->insert( hash , 9999999999999 );
            }
            minimums->insert( hash , qMin( minimums->value( hash ) , io_value.toDouble( 0 ) ) );
        }
    }

}

