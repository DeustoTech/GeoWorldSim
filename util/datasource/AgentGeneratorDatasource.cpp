#include "AgentGeneratorDatasource.h"
#include "../../util/datasource/DatasourceReader.h"
#include "../../agent/Agent.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include <QObject>


AgentGeneratorDatasource::AgentGeneratorDatasource(QJsonObject json, QString url)
{

    GWSDatasourceReader* agentReader = new GWSDatasourceReader( url );

    agentReader->connect( agentReader , &GWSDatasourceReader::dataValueReadSignal , [json]( QJsonObject data ){

        QJsonObject template_to_be_constructed = json;
        foreach ( QString key , data.keys()) {
            template_to_be_constructed.insert( key , data.value( key ) );
        }

        if ( !template_to_be_constructed.isEmpty() ){

            QSharedPointer<GWSAgent> humans = GWSObjectFactory::globalInstance()->fromJSON( template_to_be_constructed ).dynamicCast<GWSAgent>();
            GWSExecutionEnvironment::globalInstance()->registerAgent( humans );

        }

    } );


}
