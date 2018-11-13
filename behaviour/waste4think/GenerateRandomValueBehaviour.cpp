#include "GenerateRandomValueBehaviour.h"
#include "../../util/random/UniformDistribution.h"


QString GenerateRandomValueBehaviour::STORE_RANDOM_VALUE_AS = "store_random_value_as";
QString GenerateRandomValueBehaviour::NEXTS = "nexts";

GenerateRandomValueBehaviour::GenerateRandomValueBehaviour() : GWSBehaviour (){

}



QStringList GenerateRandomValueBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    double random_number = UniformDistribution::uniformDistribution( );
    agent->setProperty( this->getProperty( STORE_RANDOM_VALUE_AS ).toString() , random_number );
    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;


}
