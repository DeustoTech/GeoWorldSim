#include "GenerateRandomValueBehaviour.h"
#include "../../util/random/UniformDistribution.h"


QString GenerateRandomValueBehaviour::STORE_RANDOM_VALUE_AS = "store_random_value_as";
QString GenerateRandomValueBehaviour::MULTIPLIER = "factor_to_multiply_random_value_with";
QString GenerateRandomValueBehaviour::NEXTS = "nexts";

GenerateRandomValueBehaviour::GenerateRandomValueBehaviour() : GWSBehaviour (){

}



QJsonArray GenerateRandomValueBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double random_number = UniformDistribution::uniformDistribution( );
    random_number = random_number * this->getProperty( MULTIPLIER ).toDouble();
    agent->setProperty( this->getProperty( STORE_RANDOM_VALUE_AS ).toString() , random_number );
    return this->getProperty( NEXTS ).toArray();


}