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
    QJsonArray* train_json_inputs = new QJsonArray();
    QJsonArray* train_json_outputs = new QJsonArray();

    bool inputs_finished = false;
    bool outputs_finised = false;

    // Create DataSourceReader from URL
    GWSDatasourceReader* idsr = new GWSDatasourceReader( this->url );

    // Connect DSR to load data into QJsonArray:
    idsr->connect( idsr , &GWSDatasourceReader::dataValueReadSignal , [train_json_inputs]( QJsonObject read_json ){
           train_json_inputs->append( read_json );
   });

   idsr->connect( idsr , &GWSDatasourceReader::dataReadingFinishedSignal , [&inputs_finished](){
       inputs_finished = true;
       if( outputs_finised ){ this->startTraining(); }
    });

   // Create DataSourceReader from URL
   GWSDatasourceReader* odsr = new GWSDatasourceReader( this->url );

   // Connect DSR to load data into QJsonArray:
   odsr->connect( odsr , &GWSDatasourceReader::dataValueReadSignal , [train_json_outputs]( QJsonObject read_json ){
          train_json_outputs->append( read_json );
  });

  odsr->connect( odsr , &GWSDatasourceReader::dataReadingFinishedSignal , [&outputs_finised](){
      outputs_finised = true;
      if( inputs_finished ){ this->startTraining(); }
   });

   idsr->startReading();
   odsr->startReading();

}

void NeuralNetworkFromDatasource::startTraining(){

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
