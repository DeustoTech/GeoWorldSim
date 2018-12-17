

#include <ios>
#include <iostream>
#include <iomanip>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>


// Utils
#include "../../../util/neural_network/NeuralNetwork.h"


// Test function that demonstrates usage of the fann C++ wrapper
void electricTravelling_impactTool()
{

    /*
       NEURAL NETWORK:
       Get NN training data from GWS DataSources from URL  */

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








}

/* Startup function. Syncronizes C and C++ output, calls the test function
   and reports any exceptions */
int main(int argc, char **argv)
{
    try
    {
        QCoreApplication app( argc , argv );
        std::ios::sync_with_stdio(); // Syncronize cout and printf output

        // Check if trained network file exists:
        QString save_file = argv[3];
        GWSNeuralNetwork* neural_network;

        if ( !QFile( save_file ).exists() ){

            QString inputs_file = argv[1];
            QString outputs_file = argv[2];

            neural_network = new GWSNeuralNetwork( 0.7, 0.001, 30000, 100 );
            neural_network->trainFromFile( inputs_file , outputs_file );
            neural_network->save( save_file.toStdString() );

        }

        else {

            neural_network->load( save_file.toStdString() );

        }



    }
    catch (...)
    {
        qWarning() << endl << "Abnormal exception." << endl;
    }
    return 0;
}

/******************************************************************************/

