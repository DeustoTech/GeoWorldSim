#include "CreateTrainedNeuralNetwork.h"

#include "../../util/neural_network/NeuralNetworkFromDatasource.h"

QString CreateTrainedNeuralNetwork::URL = "training_url";
QString CreateTrainedNeuralNetwork::NEXT = "next";

CreateTrainedNeuralNetwork::CreateTrainedNeuralNetwork() : GWSBehaviour ()
{

}


QJsonArray CreateTrainedNeuralNetwork::behave(){


    NeuralNetworkFromDatasource* neural_network = new NeuralNetworkFromDatasource( this->getProperty( URL ).toString() );
    neural_network->generateDataset( 0 , 6999 , 6999 , 2999 );
    //neural_network->createNetwork();
    //neural_network->train();
    //neural_network->test();

    return this->getProperty( NEXT ).toArray();
}
