#include "PropertyStatisticsBehaviour.h"

#include "../../environment/agent_environment/AgentEnvironment.h"

QString PropertyStatisticsBehaviour::AGENTS_TYPE = "agents_type";
QString PropertyStatisticsBehaviour::AGENTS_FILTER = "agents_filter";
QString PropertyStatisticsBehaviour::AGENTS_PROPERTY_NAME = "agents_property_name";
QString PropertyStatisticsBehaviour::STORE_TOTAL_AS = "store_total_as";
QString PropertyStatisticsBehaviour::NEXTS = "nexts";

PropertyStatisticsBehaviour::PropertyStatisticsBehaviour() : GWSBehaviour(){

}

/**********************************************************************
 METHODS
**********************************************************************/

QPair< double , QJsonArray > PropertyStatisticsBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    QString agents_type = this->getProperty( AGENTS_TYPE ).toString();
    QJsonObject agents_filter = this->getProperty( AGENTS_FILTER ).toObject();
    QList< QSharedPointer<GWSAgent> > agents = GWSAgentEnvironment::globalInstance()->getByClass( agents_type );

    double count = 0;
    double total_sum = 0;
    double average = 0;

    foreach (QSharedPointer<GWSAgent> agent , agents) {
        
        bool valid = true;

        foreach( QString key , agents_filter.keys() ){
           // qDebug() << key << agents_filter.value( key ) << agent->getProperty( key );

            if ( ( agent->getProperty( key ).isNull() ) || ( agent->getProperty( key ) != agents_filter.value( key ) ) ){
                valid = false;
                break;
            }
        }

        if( !valid ){ continue; }
        
        double val = agent->getProperty( this->getProperty( AGENTS_PROPERTY_NAME ).toString() ).toDouble();
        if( average == 0 ){ average = val; }

        average = (average + val) / 2;
        total_sum += val;
        count++;
    }

    if( count > 0 ){ average = total_sum / count; }

    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_count" , count );
    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_sum" , total_sum );
    agent->setProperty( this->getProperty( STORE_TOTAL_AS ).toString( "statistics_total" ) + "_avg", average );

    return QPair< double , QJsonArray >( this->getProperty( BEHAVIOUR_DURATION ).toDouble() , this->getProperty( NEXTS ).toArray() );
}
