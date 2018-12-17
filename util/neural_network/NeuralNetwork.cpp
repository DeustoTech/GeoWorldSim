#include "NeuralNetwork.h"

#include <QJsonObject>
#include <QDebug>
#include <QFile>

using namespace std;

GWSNeuralNetwork::GWSNeuralNetwork(double learning_rate, double desired_error, int max_iterations, int iterations_between_reports ) : QObject( ){

    this->learning_rate = learning_rate;

    this->num_hidden = num_hidden;

    this->desired_error = desired_error;
    this->max_iterations = max_iterations;
    this->iterations_between_reports = iterations_between_reports;


}

GWSNeuralNetwork::GWSNeuralNetwork( QString training_file ){
    FANN::training_data data;

}

GWSNeuralNetwork::~GWSNeuralNetwork(){
    if( this->input_positions ){ delete this->input_positions; }
    if( this->input_minimums ){ delete this->input_minimums; }
    if( this->input_maximums ){ delete this->input_maximums; }
    if( this->output_positions ){ delete this->output_positions; }
    if( this->output_minimums ){ delete this->output_minimums; }
    if( this->output_maximums ){ delete this->output_maximums; }
}

/**********************************************************************
  METHODS
**********************************************************************/

void GWSNeuralNetwork::trainFromFile(QString inputs_file_path, QString outputs_file_path) {


    QList< QList< QPair< QString , QVariant> > > inputs;

    {
        QFile file( inputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO FILE";
            return;
        }

        QTextStream in(&file);
        QMap<QString , int> headers;

        while( !in.atEnd() ) {

            QString line = in.readLine();

            // First read headers
            if( headers.isEmpty() ){
                QStringList columns = line.split(';');
                foreach( QString c , columns ){
                    // Remove surrounding quotes
                    //c.replace( "\"" , "" );
                    headers.insert( c.replace( "\"" , "" ) , columns.indexOf( c ) );
                }

                // Next reads are data
            } else {

                QList< QPair< QString, QVariant > > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.append( QPair< QString, QVariant>( headers.key( i ) , value ) );
                }

                inputs.insert( 0 , row );
            }
        }

        file.close();
    }


    QList< QList< QPair< QString , QVariant> > > outputs;
    {
        QFile file( outputs_file_path );

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO FILE";
            return;
        }

        QTextStream in(&file);
        QMap<QString , int> headers;

        while( !in.atEnd() ) {

            QString line = in.readLine();

            // First read headers
            if( headers.isEmpty() ){
                QStringList columns = line.split(';');
                foreach( QString c , columns ){
                    headers.insert( c.replace( "\"" , "" ) , columns.indexOf( c ) );
                }

                // Next reads are data
            } else {

                QList< QPair< QString, QVariant > > row;
                QStringList columns = line.split(';');
                for(int i = 0; i < columns.size() ; i++ ){

                    QString string_value = columns.at(i);
                    // Remove surrounding quotes
                    string_value.replace( "\"" , "" );

                    bool ok;
                    QVariant value = string_value.toDouble( &ok );
                    if( !ok ){ value = string_value; }

                    row.append( QPair< QString, QVariant>( headers.key( i ) , value ) );
                }

                outputs.insert( 0 , row );
            }
        }

        file.close();
    }

    this->train( inputs , outputs );
}


/* Select random input from file to test the network */
void GWSNeuralNetwork::randomLine( QString inputs_file_path ){

       QFile input_file( inputs_file_path );

       if(!input_file.open(QIODevice::ReadOnly)) {
            qWarning() << "NO FILE";
            return;
       }

    // Get number of lines in input file:
       QTextStream in(&input_file);
       int line_number = 0;
       while( !in.atEnd() ) {

           QString line = in.readLine();
           ++line_number;
        }

        // Select a line randomly:
       int random_line = qrand() % line_number;

       while( !in.atEnd() ) {

           QString line = in.readLine();
           ++line_number;
           if ( line_number == random_line ){
               QStringList columns = line.split(';');
               for(int i = 0; i < columns.size() ; i++ ){

                   QString string_value = columns.at(i);
                   // Remove surrounding quotes
                   string_value.replace( "\"" , "" );

                   bool ok;
                   QVariant value = string_value.toDouble( &ok );
                   if( !ok ){ value = string_value; }

                   row.append( QPair< QString, QVariant>( headers.key( i ) , value ) );
               }
           }
        }



}

/* Train the network on given input and output arrays  */

void GWSNeuralNetwork::trainFromJSON( QJsonArray input_train_dataset , QJsonArray output_train_dataset ){

    QList< QList< QPair< QString , QVariant> > > inputs;
    foreach( QJsonValue r , input_train_dataset ){
        QJsonObject row = r.toObject();
        QList< QPair< QString , QVariant> > qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.append( QPair<QString , QVariant>( key , row.value( key ).toVariant() ) );
        }
        inputs.insert( 0 , qpair_list );
    }

    QList< QList< QPair< QString , QVariant> > >  outputs;
    foreach( QJsonValue r , output_train_dataset ){
        QJsonObject row = r.toObject();
        QList< QPair< QString , QVariant> > qpair_list;
        foreach( QString key , row.keys() ){
            qpair_list.append( QPair<QString , QVariant>( key , row.value( key ).toVariant() ) );
        }
        outputs.insert( 0 , qpair_list );
    }

    this->train( inputs , outputs );

}

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


    // Initialize input set:
    fann_type** fann_inputs = new fann_type* [ input_train_dataset.size() ]; // Row amount of the dataset

    for ( int i = 0 ; i < input_train_dataset.size() ; i++ ){ // Columns that represent the inputs of the dataset

        fann_inputs[i] = new fann_type[ this->input_positions->keys().size() ];

        // Init all positions to 0
        for(int j = 0 ; j < this->input_positions->keys().size() ; j++ ){
            fann_inputs[i][j] = 0;
        }

        // Go row by row setting the inputs
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QList< QPair< QString , QVariant> > row = input_train_dataset.at( i );
        for( int j = 0 ; j < row.size() ; j++ ) {

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            double value_double = 0;

            if( value_variant.type() == QVariant::String ){
                hash = hash + ":" + value_variant.toString();
                value_double = 1;
            } else {
                value_double = value_variant.toDouble();
            }

            double min = this->input_minimums->value( hash );
            double max = this->input_maximums->value( hash );
            value_double = ( value_double - min ) / ( max - min );

            int position = this->input_positions->value( hash , -1 );
            if( position > -1 ){
                fann_inputs[i][position] = value_double;
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

        // Go row by row setting the inputs
        // Scaling rule:  x_new = (x - x1_min) / (x1_max - x1_min)
        QList< QPair< QString , QVariant> > row = output_train_dataset.at( i );
        for( int j = 0 ; j < row.size() ; j++ ) {

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            double value_double = 0;

            if( value_variant.type() == QVariant::String ){
                hash = hash + ":" + value_variant.toString();
                value_double = 1;
            } else {
                value_double = value_variant.toDouble();
            }

            double min = this->output_minimums->value( hash );
            double max = this->output_maximums->value( hash );
            value_double = ( value_double - min ) / ( max - min );

            int position = this->output_positions->value( hash , -1 );
            if( position > -1 ){
                fann_inputs[i][position] = value_double;
            }

        }

    }

    // Divide data into training and testing batches:
    this->train_data.set_train_data( input_train_dataset.size() , this->input_positions->keys().size() , fann_inputs , this->output_positions->keys().size() , fann_outputs );

    qDebug() << "Creating network." << endl;

    // this->net.create_standard( this->input_positions->size() + this->output_positions->size(), this->input_positions->size(), this->num_hidden, this->output_positions->size());

    // Crash when using create_standard method if total layers > 3. Using create_standard_array instead:
    unsigned int layers[4] = { (unsigned int)this->input_positions->keys().size()  , 8 , 9 ,  (unsigned int)this->output_positions->keys().size() };
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
    //this->net.save( "/home/maialen/Escritorio/WorkSpace/FannExamples/XOR_QJsonInput/network.net" );

}


/*  Save the network to given file name  */


void GWSNeuralNetwork::save( const std::string trained_network_filename ){


    // Save trained network:
    this->net.save( trained_network_filename );

}



/* Load trained network */
void GWSNeuralNetwork::load( const std::string trained_network_filename ){


    // Save trained network:
    this->net.create_from_file( trained_network_filename );

}

/*  Run the network on a given input set  */

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

/*  Allocate positions to input fields and get maximum and minimum values
    for eventual normalization (FANN data formatting requirement)        */

void GWSNeuralNetwork::generatePositions(QList< QList< QPair< QString , QVariant> > > data_rows, QMap<QString, int> *positions, QMap<QString, double> *maximums, QMap<QString, double> *minimums){

    // Give positions to the outputs
    for( int i = 0 ; i < data_rows.size() ; i ++ ) {

        QList< QPair< QString , QVariant> > row = data_rows.at(i);

        for( int j = 0 ; j < row.size() ; j ++ ){

            QPair< QString , QVariant > pair = row.at( j );
            QString hash = pair.first;
            QVariant value_variant = pair.second;
            double value_double = 0;

            if( value_variant.type() == QVariant::String ){
                hash = hash + ":" + value_variant.toString();
                value_double = 1;
            } else {
                value_double = value_variant.toDouble();
            }

            // Initialize position
            if( !positions->keys().contains( hash ) ){
                positions->insert( hash , positions->keys().size() );
            }

            // Initialize maximum
            if( !maximums->keys().contains( hash ) ){
                maximums->insert( hash , -9999999999999 );
            }
            maximums->insert( hash , qMax( maximums->value( hash ) , value_double ) );

            // Initialize minimum
            if( !minimums->keys().contains( hash ) ){
                minimums->insert( hash , 9999999999999 );
            }
            minimums->insert( hash , qMin( minimums->value( hash ) , value_double ) );
        }
    }

}

