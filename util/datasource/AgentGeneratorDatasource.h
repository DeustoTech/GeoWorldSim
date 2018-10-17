#ifndef AGENTGENERATORDATASOURCE_H
#define AGENTGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class AgentGeneratorDatasource : public QObject
{

public:
    AgentGeneratorDatasource();

    QJsonObject template_agent;
};

#endif // AGENTGENERATORDATASOURCE_H
