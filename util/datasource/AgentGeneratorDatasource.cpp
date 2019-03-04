#include "AgentGeneratorDatasource.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../agent/Agent.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../app/App.h"
#include "../../agent/Agent.h"

#include <QObject>
#include <QTimer>

GWSAgentGeneratorDatasource::GWSAgentGeneratorDatasource(QJsonObject json, QString scenario_id , QString entity_type, int amount ) : QObject ()
{

    if( json.isEmpty() ){
        qCritical() << "Empty JSON template to join with the datasource";
        return;
    }

    GWSDatasourceReader* agentReader = new GWSDatasourceReader( scenario_id , entity_type, amount );
    agentReader->connect( agentReader , &GWSDatasourceReader::dataReadingFinishedSignal , [this](){
        emit this->dataReadingFinishedSignal();
    });
    agentReader->connect( agentReader , &GWSDatasourceReader::dataValueReadSignal , [this , json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = this->joinJSON( json , data );
        if ( !template_to_be_constructed.isEmpty() ){
            QSharedPointer<GWSAgent> agent = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSAgent>();

            /*if( !agent->getProperty( GWSExecutionEnvironment::START_TIME ).isNull() ){
               qint64 currentDateTimeMsecs = GWSTimeEnvironment::globalInstance()->getCurrentDateTime(); // returns milliseconds
               qint64 startAfterMsecs = agent->getProperty( GWSExecutionEnvironment::START_TIME ).toDouble();
               qint64 start_in_msecs = qMax( (qint64)0 , startAfterMsecs - currentDateTimeMsecs );
               agent->setProperty( GWSExecutionEnvironment::START_TIME , start_in_msecs );
               QTimer::singleShot( start_in_msecs, [ agent  ]() {
                   agent->run() ;

               } );
            }

            else {
                agent->setProperty( GWSExecutionEnvironment::START_TIME , GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent ) );

            }*/
        }

    });

    agentReader->startReading();

}

QJsonObject GWSAgentGeneratorDatasource::joinJSON(QJsonObject json_template, QJsonObject json_data){

    foreach ( QString key , json_data.keys() ) {

        QJsonValue value = json_data.value( key );

        // Extract THE_REAL_VALUE from { type : "string|number|..." , metadata : {} , value : THE_REAL_VALUE }
        if( value.toObject().keys().contains("value") ){ value = value.toObject().value("value"); }

        // IF Value is a complex object, recursively call joinJSON
        if( value.isObject() ){
            json_template.insert( key , this->joinJSON( json_template.value( key ).toObject() , value.toObject() ) );
        } else {
            json_template.insert( key , value );
        }

    }

    return json_template;
}
