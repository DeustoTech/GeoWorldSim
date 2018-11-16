#include "ChooseRandomValueFromSetBehaviour.h"

QString ChooseRandomValueFromSetBehaviour::NEXTS = "nexts";
QString ChooseRandomValueFromSetBehaviour::AGENT_PROPERTY_TO_ASSIGN = "property_to_assign_random_value";
QString ChooseRandomValueFromSetBehaviour::VALUE_SET = "set_to_choose_from";


ChooseRandomValueFromSetBehaviour::ChooseRandomValueFromSetBehaviour() : GWSBehaviour(){

}


QStringList ChooseRandomValueFromSetBehaviour::behave(){


    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Read property and value set from behaviour input:
    QJsonArray set_to_choose_from = this->getProperty( VALUE_SET ).toJsonArray();
    QString property = this->getProperty( AGENT_PROPERTY_TO_ASSIGN ).toString();

    // Pick set element randomly:
    QStringList set;
    foreach( QJsonValue n , this->getProperty( VALUE_SET ).toJsonArray() ){
        set.append( n.toString() );
    }
    QString random_choice = set.at( qrand() % set_to_choose_from.size() );
    agent->setProperty( property , random_choice);
    return this->getNexts( NEXTS );




}
