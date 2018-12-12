#include "NeuralNetworkFromDatasource.h"

#include "../../util/datasource/DatasourceReader.h"

#include <QJsonArray>

NeuralNetworkFromDatasource::NeuralNetworkFromDatasource( QString url ) : QObject(){

    // URL example:
    // "http://datasources.geoworldsim.com/api/datasource/755fb482-c2aa-4c19-aa3d-d1456cf248a0/read"

    // Set Neural Network parameters:
    this->url = url;

}

void NeuralNetworkFromDatasource::generateDataset( int train_pos , int train_length , int test_pos , int test_length ){

    // Create empty QJsonArray to store data from DataSource
    QJsonArray* train_json_data = new QJsonArray();

    // Create DataSourceReader from URL
    GWSDatasourceReader* dsr = new GWSDatasourceReader( this->url );

    // Connect DSR to load data into QJsonArray:
    dsr->connect( dsr , &GWSDatasourceReader::dataValueReadSignal , [train_json_data]( QJsonObject read_json ){

           QJsonObject row;
           QJsonArray inputs;
           foreach(QString key , read_json.keys()){
               if( key != "2" ){
                   inputs.append( read_json.value( key ).toDouble() );
               }
           }
           row.insert("inputs" , inputs);

           QJsonArray outputs;
           outputs.append( read_json.value("2").toDouble() );
           row.insert( "outputs" , outputs );

           train_json_data->append( row );
   });


   dsr->connect( dsr , &GWSDatasourceReader::dataReadingFinishedSignal , [this, train_json_data , train_pos , train_length , test_pos , test_length ](){

       // Get additional NN parameters from DataSource QJsonArray:
       this->num_data = train_json_data->size();
       this->num_input = train_json_data->at(0).toObject().value("inputs").toArray().size();
       this->num_output = train_json_data->at(0).toObject().value("outputs").toArray().size();

       // Initialize input set:
       this->input_set = new double* [this->num_data];

       for ( unsigned int i = 0 ; i < this->num_data ; i++ ){

            this->input_set[i] = new double[this->num_input];

            for( unsigned int j = 0 ; j < this->num_input ; j++ ){
               QJsonObject obj = train_json_data->at(i).toObject();
               double element = obj.value( "inputs" ).toArray().at(j).toDouble();
               this->input_set[ i ][ j ] = element;
           }
        }

        // Initialize and fill output set:
        this->output_set = new double* [this->num_data];

        for ( unsigned int i = 0 ; i < this->num_data ; i++ ){

            this->output_set[i] = new double[this->num_output];

            for( unsigned int j = 0 ; j < this->num_output ; j++ ){
               QJsonObject obj = train_json_data->at(i).toObject();
               double element = obj.value( "outputs" ).toArray().at(j).toDouble();
               this->output_set[ i ][ j ] = element;
            }
         }

        // Divide data into training and testing batches:
        this->train_data.set_train_data( this->num_data , this->num_input , this->input_set , this->num_output , this->output_set );
        this->train_data.subset_train_data( train_pos , train_length );

        this->test_data.set_train_data( this->num_data , this->num_input , this->input_set , this->num_output , this->output_set );
        this->test_data.subset_train_data( test_pos , test_length);


        this->learning_rate = 0.7;
        this->num_layers = 3;
        this->num_hidden = 13;
        this->desired_error = 0.001;
        this->max_iterations = 300000;
        this->iterations_between_reports = 1000;

        qDebug() << "Creating network." << endl;

        this->net.create_standard( this->num_layers, this->num_input, this->num_hidden, this->num_output);

        this->net.set_learning_rate( this->learning_rate );

        this->net.set_activation_steepness_hidden(1.0);
        this->net.set_activation_steepness_output(1.0);

        this->net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC_STEPWISE);
        this->net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC_STEPWISE);

        // Set additional properties such as the training algorithm
        //net.set_training_algorithm(FANN::TRAIN_QUICKPROP);

        // Print NN parameters
        this->net.print_parameters();


        qDebug() << endl << "Training network." << endl;

        // Train the network with the training dataset:
        this->net.init_weights( this->train_data );
        this->net.train_on_data( this->train_data , this->max_iterations , this->iterations_between_reports , this->desired_error );

        // Save trained network:
        this->net.save( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/Waste4Think_V1/trained_network.net");


    });


   dsr->startReading();

}

void NeuralNetworkFromDatasource::test(){

       qDebug() << endl << "Testing network." << endl;

       // Test on training data:
       this->net.test_data( this->train_data );
       qDebug() << "Train data MSE = " << net.get_MSE();
       net.reset_MSE();

       // Test on test data:
       this->net.test_data( this->test_data );
       qDebug() << "Test data MSE = " << net.get_MSE();
       net.reset_MSE();

}


void NeuralNetworkFromDatasource::run( double* input ){

    //double* input = new double [this->num_input];
    this->output = this->net.run( input );


}
