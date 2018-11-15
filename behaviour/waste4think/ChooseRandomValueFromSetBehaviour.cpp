#include "ChooseRandomValueFromSetBehaviour.h"

QString ChooseRandomValueFromSetBehaviour::NEXTS = "nexts";
QString ChooseRandomValueFromSetBehaviour::AGENT_PROPERTY_TO_ASSIGN = "property_to_assign_random_value";
QString ChooseRandomValueFromSetBehaviour::VALUE_SET = "set_to_choose_from";


ChooseRandomValueFromSetBehaviour::ChooseRandomValueFromSetBehaviour() : GWSBehaviour(){

}


QStringList ChooseRandomValueFromSetBehaviour::behave(){


    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Read property and value set from behaviour input:
    QStringList set_to_choose_from = this->getProperty( VALUE_SET ).toStringList();
    QString property = this->getProperty( AGENT_PROPERTY_TO_ASSIGN ).toString();

    // Pick set element randomly:
    QString random_choice = set_to_choose_from.at( qrand() % set_to_choose_from.size() );
    agent->setProperty( property , random_choice);

    QStringList nexts = this->getProperty( NEXTS ).toStringList();
    return nexts;




}
