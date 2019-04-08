#include "PropertyStatisticsBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

QString PropertyStatisticsBehaviour::AGENTS_TYPE = "agents_type";
QString PropertyStatisticsBehaviour::AGENTS_PROPERTY_NAME = "agents_property_name";
QString PropertyStatisticsBehaviour::STORE_TOTAL_AS = "store_total_as";
QString PropertyStatisticsBehaviour::NEXTS = "nexts";

PropertyStatisticsBehaviour::PropertyStatisticsBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 METHODS
**********************************************************************/

QJsonArray PropertyStatisticsBehaviour::behave(){

    QString agents_type = this->getProperty( AGENTS_TYPE ).toString();
    QList< QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( agents_type );

    double count = agents.size();
    double total_sum = 0;
    double average = 0;
    foreach (QSharedPointer<GWSAgent> agent , agents) {
        double val = agent->getProperty( this->getProperty( AGENTS_PROPERTY_NAME ).toString() ).toDouble();
        if( average == 0 ){ average = val; }

        average = (average + val) / 2;
        total_sum += val;
    }

    if( count > 0 ){ average = total_sum / count; }

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_count" , count );
    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_sum" , total_sum );
    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_avg", average );

    return this->getProperty( NEXTS ).toArray();
}
