

#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>


// Utils
#include "../../../util/datasource/DatasourceReader.h"
#include "../../../util/neural_network/NeuralNetwork.h"
#include "../../../util/api/APIDriver.h"


// Test function that demonstrates usage of the fann C++ wrapper
void electricTravelling_impactTool()
{

    /*
       NEURAL NETWORK:
       Get NN training data from GWS DataSources from URL  */

    GWSDatasourceReader* idsr = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/fa6eeb69-b9c6-4293-9f6d-ceb66f64a72a/read" );
    GWSDatasourceReader* odsr = new GWSDatasourceReader( "http://datasources.geoworldsim.com/api/datasource/ae53c8d8-fcdb-40db-83cf-0cfbfd0b3e48/read" );

    // Create empty QJsonArray to store data from DataSource
    QJsonArray* train_json_inputs = new QJsonArray();
    QJsonArray* train_json_outputs = new QJsonArray();

    // Connect training input DSR to load data into QJsonArray:
    /*idsr->connect( idsr , &GWSDatasourceReader::dataValueReadSignal , [train_json_inputs]( QJsonObject read_json ){
           train_json_inputs->append( read_json );
    });

    idsr->connect( idsr , &GWSDatasourceReader::dataReadingFinishedSignal , [odsr , train_json_inputs , train_json_outputs ](){
         if( odsr->downloadedFinished() ){

            GWSNeuralNetwork* neural_network = new GWSNeuralNetwork( 0.7, 0.001, 30000, 100 );
            neural_network->train( *train_json_inputs , *train_json_outputs );
            neural_network->save( "../../../usecase/ElectricTravelling/ElectricTravelling_ImpactTool/ET_trainedNetwork.net");

            // 10448: HC	RUR/MW/130/Heavy	+2%	151-250cc	MC 4S Euro-0	124.2134170532
            // 10276: HC	RUR/MW/130/Heavy	-6%	≤50cc	Moped-EU2	30

            QJsonObject test_input = {
                                     { "Component" , "HC" },
                                     { "TrafficSit" , "RUR/MW/130/Heavy" } ,
                                     { "Gradient" ,  "-6%" } ,
                                     { "SizeClasse" , "≤50cc" },
                                     { "EmConcept" , "Moped-EU2" },
                                     { "V" , 30 }
                                     };
            qDebug() << test_input;
            QJsonObject result = neural_network->run( test_input );
            qDebug() << 2;

        }
    });


    odsr->connect( odsr , &GWSDatasourceReader::dataValueReadSignal , [train_json_outputs]( QJsonObject read_json ){
           train_json_outputs->append( read_json );
    });

    odsr->connect( odsr , &GWSDatasourceReader::dataReadingFinishedSignal , [idsr , train_json_inputs , train_json_outputs ](){
        if( idsr->downloadedFinished() ){

            GWSNeuralNetwork* neural_network = new GWSNeuralNetwork( 0.7, 0.001, 30000, 100 );
            neural_network->train( *train_json_inputs , *train_json_outputs );
        }
     });


    idsr->startReading();
    odsr->startReading();*/


    // Train from files:
    GWSNeuralNetwork* neural_network = new GWSNeuralNetwork( 0.7, 0.001, 30000, 100 );
    neural_network->trainFromFile( "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/MC_OLD/EFA_HOT_Subsegm_MC_allpollutants_rural_allLevelsOfService_LESSColumns_INPUTS.csv" , "/home/maialen/Escritorio/WorkSpace/FILES/HBEFA/MC_OLD/EFA_HOT_Subsegm_MC_allpollutants_rural_allLevelsOfService_LESSColumns_OUTPUTS.csv" );
    neural_network->save( "/home/maialen/Escritorio/WorkSpace/GwoWorldSim/usecase/ElectricTravelling/ElectricTravelling_ImpactTool/trained_network.net" );



}

/* Startup function. Syncronizes C and C++ output, calls the test function
   and reports any exceptions */
int main(int argc, char **argv)
{
    try
    {
        QCoreApplication app( argc , argv );
        std::ios::sync_with_stdio(); // Syncronize cout and printf output
        electricTravelling_impactTool();
        app.exec();
    }
    catch (...)
    {
        qWarning() << endl << "Abnormal exception." << endl;
    }
    return 0;
}

/******************************************************************************/

