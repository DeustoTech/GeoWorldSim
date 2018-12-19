#include "NeuralNetwork.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QTextStream>

using namespace std;

GWSNeuralNetwork::GWSNeuralNetwork() : GWSIntelligence (){
}

GWSNeuralNetwork::~GWSNeuralNetwork(){
}

/**********************************************************************
  METHODS
**********************************************************************/

void GWSNeuralNetwork::setParameters(double learning_rate, double desired_error, int max_iterations, int iterations_between_reports){
    this->learning_rate = learning_rate;
    this->desired_error = desired_error;
    this->max_iterations = max_iterations;
    this->iterations_between_reports = iterations_between_reports;
}

/* Train the network on given input and output arrays  */

void GWSNeuralNetwork::train(QList< QList< QPair< QString , QVariant> > > input_train_dataset, QList< QList< QPair< QString , QVariant> > >  output_train_dataset ){

    Q_ASSERT( input_train_dataset.size() == output_train_dataset.size() );

    this->input_positions = new QMap<QString , int>();
    this->input_maximums = new QMap<QString , double>();
    this->input_minimums = new QMap<QString , double>();
    this->generatePositions( input_train_dataset  , this->input_positions , this->input_maximums , this->input_minimums );

    this->output_positions = new QMap<QString , int>();
    this->output_maximums = new QMap<QString , double>();
    this->output_minimums = new QMap<QString , double>();
    this->generatePositions( output_train_dataset , this->output_positions , this->output_maximums , this->output_minimums );

    // Create training_data
    struct fann_train_data* data = fann_create_train( input_train_dataset.size() , this->input_positions->keys().size() , this->output_positions->keys().size() );

    // Initialize and fill input set:
    for ( int i = 0 ; i < input_train_dataset.size() ; i++ ){ // Columns that represent the inputs of the dataset

        // Init all positions to 0
        for(int j = 0 ; j < this->input_positions->keys().size() ; j++ ){
            data->input[i][j] = 0;
        }

        // Go row by row setting the inputs
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QList< QPair< QString , QVariant> > row = input_train_dataset.at( i );
        for( int j = 0 ; j < row.size() ; j++ ) {

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            hash = this->getIOName( hash , value_variant );

            // Normalize only if value_variant is double AND not zero:

            double value_double = this->normalizeIO( value_variant , hash , this->input_maximums , this->input_minimums );

            int position = this->input_positions->value( hash , -1 );
            if( position > -1 ){
                data->input[i][position] = value_double;
            }
        }
    }

    // Initialize and fill output set:
    for ( int i = 0 ; i < output_train_dataset.size() ; i++ ){  // Columns tha represent the outputs of the dataset

        // Init all positions to 0
        for(int j = 0 ; j < this->output_positions->keys().size() ; j++ ){
            data->output[i][j] = 0;
        }

        // Go row by row setting the inputs
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QList< QPair< QString , QVariant> > row = output_train_dataset.at( i );
        for( int j = 0 ; j < row.size() ; j++ ) {

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            hash = this->getIOName( hash , value_variant );
            double value_double = this->normalizeIO( value_variant , hash , this->output_maximums , this->output_minimums );

            int position = this->output_positions->value( hash , -1 );
            if( position > -1 ){
                data->output[i][position] = value_double;
            }
        }
    }

    qDebug() << "Creating network." << endl;

    this->train_data = FANN::training_data();
    this->train_data.set_train_data( data->num_data , data->num_input , data->input , data->num_output , data->output );

    //this->net.create_standard( 2 , this->input_positions->size(),  this->output_positions->size());

    // Crash when using create_standard method if total layers > 3. Using create_standard_array instead:
    unsigned int layers[4] = { (unsigned int)this->input_positions->keys().size() ,  74 , 32 , (unsigned int)this->output_positions->keys().size() };
    this->net.create_standard_array(4, layers);

    this->net.set_learning_rate( this->learning_rate );
    this->net.set_activation_steepness_hidden(1.0);
    this->net.set_activation_steepness_output(1.0);

    this->net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
    this->net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

    // Set additional properties such as the training algorithm
    net.set_training_algorithm(FANN::TRAIN_BATCH);

    qDebug() << endl << "Training network." << endl;

    // Train the network with the training dataset:
    //this->net.init_weights( this->train_data );

    this->net.randomize_weights( -1 , 1 );
    this->net.train_on_data( this->train_data , this->max_iterations , this->iterations_between_reports , this->desired_error );
    //this->net.save( "/home/maialen/Escritorio/WorkSpace/FannExamples/XOR_QJsonInput/network.net" );

    this->net.print_parameters();
    //this->net.print_connections();
}


/*  Save the network to given file name  */


void GWSNeuralNetwork::save(const QString fann_trained_network_filename, const QString gws_neural_network_filename){

    // Save trained network:
    this->net.save( fann_trained_network_filename.toStdString() );
    QFile out( gws_neural_network_filename );
    if( out.open(QIODevice::ReadWrite) ) {
        QTextStream stream(&out);

        // Input positions
        {
            QString str;
            for(int i = 0 ; i < this->input_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += this->input_positions->key( i );
            }
            stream << str << endl;
        }

        // Input maximums
        {
            QString str;
            for(int i = 0 ; i < this->input_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += QString::number( this->input_maximums->value( this->input_positions->key( i ) ) );
            }
            stream << str << endl;
        }

        // Input minimums
        {
            QString str;
            for(int i = 0 ; i < this->input_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += QString::number( this->input_minimums->value( this->input_positions->key( i ) ) );
            }
            stream << str << endl;
        }

        // Output positions
        {
            QString str;
            for(int i = 0 ; i < this->output_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += this->output_positions->key( i );
            }
            stream << str << endl;
        }

        // Output maximums
        {
            QString str;
            for(int i = 0 ; i < this->output_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += QString::number( this->output_maximums->value( this->output_positions->key( i ) ) );
            }
            stream << str << endl;
        }

        // Output minimums
        {
            QString str;
            for(int i = 0 ; i < this->output_positions->values().size() ; i++){
                if( !str.isEmpty() ){ str += ";"; }
                str += QString::number( this->output_minimums->value( this->output_positions->key( i ) ) );
            }
            stream << str << endl;
        }
    }

}

/* Load trained network */
void GWSNeuralNetwork::load(const QString fann_trained_network_filename, const QString gws_neural_network_filename){

    // Load trained network:
    this->net.create_from_file( fann_trained_network_filename.toStdString() );

    QFile in( gws_neural_network_filename );
    if( in.open(QIODevice::ReadWrite) ) {

        QTextStream stream(&in);


        // First line = input_positions
        {
            QString line = stream.readLine();
            if( this->input_positions ){ delete this->input_positions; }
            this->input_positions = new QMap<QString , int>();
            foreach( QString key , line.split(";") ){
                this->input_positions->insert( key , this->input_positions->size() );
            }

        }

        // Second line = input_maximums
        {
            QString line = stream.readLine();
            if( this->input_maximums ){ delete this->input_maximums; }
            this->input_maximums = new QMap<QString , double>();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->input_positions->key( i );
                this->input_maximums->insert( key , values.at( i ).toDouble() );
            }
        }

        // Third line = input_minimums
        {
            QString line = stream.readLine();
            if( this->input_minimums ){ delete this->input_minimums; }
            this->input_minimums = new QMap<QString , double>();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->input_positions->key( i );
                this->input_minimums->insert( key , values.at( i ).toDouble() );
            }
        }

        // Fourth line = output_positions
        {
            QString line = stream.readLine();
            if( this->output_positions ){ delete this->output_positions; }
            this->output_positions = new QMap<QString , int>();
            foreach( QString key , line.split(";") ){
                this->output_positions->insert( key , this->output_positions->size() );
            }
        }

        // Fifth line = output_maximums
        {
            QString line = stream.readLine();
            if( this->output_maximums ){ delete this->output_maximums; }
            this->output_maximums = new QMap<QString , double>();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->output_positions->key( i );
                this->output_maximums->insert( key , values.at( i ).toDouble() );
            }
        }

        // Sixth line = output_minimums
        {
            QString line = stream.readLine();
            if( this->output_minimums ){ delete this->output_minimums; }
            this->output_minimums = new QMap<QString , double>();
            QStringList values = line.split(";");
            for( int i = 0 ; i < values.size() ; i++ ){
                QString key = this->output_positions->key( i );
                this->output_minimums->insert( key , values.at( i ).toDouble() );
            }
        }
    }
}

/*  Run the network on a given input set  */

QJsonObject GWSNeuralNetwork::run( QList< QPair <QString, QVariant> > test_inputs ){

    fann_type* test_input = new fann_type[ this->input_positions->keys().size() ];

    // Init all positions to 0
    for(int j = 0 ; j < this->input_positions->keys().size() ; j++ ){
        test_input[j] = 0;
    }

    // Recorrer los input pairs:
    for ( int i = 0; i < test_inputs.size() ; i++ ){

        QPair<QString , QVariant> input_pair = test_inputs.at(i);
        QString hash = input_pair.first;
        QVariant value_variant = input_pair.second;
        hash = this->getIOName( hash , value_variant );
        double value_double = this->normalizeIO( value_variant , hash , this->input_maximums , this->input_minimums );

        int position = this->input_positions->value( hash , -1 );
        if( position > -1 ){
            test_input[i] = value_double;
        }
    }

    fann_type* test_output = this->net.run( test_input );

    QJsonObject result;

    foreach( int position , this->output_positions->values() ){

        double normalized_output = test_output[position];
        double denormalized_output = this->denormalizeIO( normalized_output , position );

        result.insert( this->output_positions->key( position ) , denormalized_output );
    }


    qDebug() << this->net.get_MSE();
    return result;
}

