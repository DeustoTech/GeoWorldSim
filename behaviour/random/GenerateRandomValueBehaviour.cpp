#include "GenerateRandomValueBehaviour.h"
#include "../../util/random/UniformDistribution.h"


QString geoworldsim::behaviour::GenerateRandomValueBehaviour::STORE_AS = "store_as";
QString geoworldsim::behaviour::GenerateRandomValueBehaviour::MULTIPLIER = "multiplier";
QString geoworldsim::behaviour::GenerateRandomValueBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::GenerateRandomValueBehaviour::GenerateRandomValueBehaviour() : Behaviour (){

}

QPair< double , QJsonArray > geoworldsim::behaviour::GenerateRandomValueBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();
    double random_number = UniformDistribution::uniformDistribution( );
    random_number = random_number * this->getProperty( MULTIPLIER ).toDouble();
    entity->setProperty( this->getProperty( STORE_AS ).toString() , random_number );
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );

}
