#include "CreateTrainedNeuralNetworkBehaviour.h"

#include "../../util/neural_network/NeuralNetworkFromDatasource.h"

QString CreateTrainedNeuralNetworkBehaviour::URL = "training_url";
QString CreateTrainedNeuralNetworkBehaviour::NEXT = "next";

CreateTrainedNeuralNetworkBehaviour::CreateTrainedNeuralNetworkBehaviour() : GWSBehaviour ()
{

}


QJsonArray CreateTrainedNeuralNetworkBehaviour::behave(){


    NeuralNetworkFromDatasource* neural_network = new NeuralNetworkFromDatasource( this->getProperty( URL ).toString() );
    neural_network->generateDataset( 0 , 6999 , 6999 , 2999 );
    //neural_network->createNetwork();
    //neural_network->train();
    //neural_network->test();

    return this->getProperty( NEXT ).toArray();
}
