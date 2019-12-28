#include "ChooseRandomValueFromSetBehaviour.h"

QString ChooseRandomValueFromSetBehaviour::NEXTS = "nexts";
QString ChooseRandomValueFromSetBehaviour::AGENT_PROPERTY_TO_ASSIGN = "property_to_assign_random_value";
QString ChooseRandomValueFromSetBehaviour::VALUE_SET = "set_to_choose_from";


ChooseRandomValueFromSetBehaviour::ChooseRandomValueFromSetBehaviour() : GWSBehaviour(){

}


QPair< double , QJsonArray > ChooseRandomValueFromSetBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Read property and value set from behaviour input:
    QJsonArray set_to_choose_from = this->getProperty( VALUE_SET ).toArray();
    QString property = this->getProperty( AGENT_PROPERTY_TO_ASSIGN ).toString();

    // Pick set element randomly:
    QJsonValue random_choice = set_to_choose_from.at( qrand() % set_to_choose_from.size() );
    agent->setProperty( property , random_choice );
    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );


}
