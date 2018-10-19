#include "AgentGeneratorDatasource.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../agent/Agent.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include <QObject>
#include "../../app/App.h"


AgentGeneratorDatasource::AgentGeneratorDatasource(QJsonObject json, QString url)
{

    GWSDatasourceReader* agentReader = new GWSDatasourceReader( url );
    agentReader->connect( agentReader , &GWSDatasourceReader::dataValueReadSignal , [json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = json;
        foreach ( QString key , data.keys()) {
            template_to_be_constructed.insert( key , data.value( key ) );
        }

        if ( !template_to_be_constructed.isEmpty() ){
            QSharedPointer<GWSAgent> agent = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( agent );
            emit GWSApp::globalInstance()->sendAgentSignal( agent->serialize() );
        }

    });


    agentReader->startReading();

}
