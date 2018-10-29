#include "AgentGeneratorDatasource.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../agent/Agent.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include <QObject>
#include "../../app/App.h"


GWSAgentGeneratorDatasource::GWSAgentGeneratorDatasource(QJsonObject json, QString url , int amount )
{

    GWSDatasourceReader* agentReader = new GWSDatasourceReader( url , amount );
    agentReader->connect( agentReader , &GWSDatasourceReader::dataValueReadSignal , [this , json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = this->joinJSON( json , data );

        if ( !template_to_be_constructed.isEmpty() ){
            QSharedPointer<GWSAgent> agent = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSAgent>();
            emit GWSApp::globalInstance()->sendAgentToSocketSignal( agent->serialize() );
        }

    });


    agentReader->startReading();

}

QJsonObject GWSAgentGeneratorDatasource::joinJSON(QJsonObject json_template, QJsonObject json_data){

    foreach ( QString key , json_data.keys() ) {

        QJsonValue value = json_data.value( key );

        // IF Value is a complex object, recursively call joinJSON
        if( value.isObject() ){
            json_template.insert( key , this->joinJSON( json_template.value( key ).toObject() , value.toObject() ) );
        } else {
            json_template.insert( key , value );
        }

    }

    return json_template;
}
