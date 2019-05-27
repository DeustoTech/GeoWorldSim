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

GWSAgentGeneratorDatasource::GWSAgentGeneratorDatasource(QJsonObject json, QString scenario_id , QString entities_type , QString entities_filter , int amount ) : QObject ()
{

    if( json.isEmpty() ){
        qCritical() << "Empty JSON template to join with the datasource";
        return;
    }

    GWSDatasourceReader* agentReader = new GWSDatasourceReader( scenario_id , entities_type , entities_filter , amount );
    agentReader->connect( agentReader , &GWSDatasourceReader::dataReadingFinishedSignal , [this](){
        emit this->dataReadingFinishedSignal();
    });
    agentReader->connect( agentReader , &GWSDatasourceReader::dataValueReadSignal , [this , json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = this->joinJSON( json , data );
        if ( template_to_be_constructed.isEmpty() ){
            return;
        }

        // Check if agent already exists
        QString new_agent_id = template_to_be_constructed.value( GWSObject::GWS_UID_PROP ).toString();
        QSharedPointer<GWSAgent> agent = GWSAgentEnvironment::globalInstance()->getByUID( new_agent_id );
        if( agent ){
            qWarning() << QString("Skipping duplicate agent (Duplicate UID %1 found)").arg( new_agent_id );
            return;
        } else {
            agent = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSAgent>();
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
