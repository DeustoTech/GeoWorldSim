#ifndef AGENTGENERATORDATASOURCE_H
#define AGENTGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class AgentGeneratorDatasource : public QObject
{

public:
    AgentGeneratorDatasource( QJsonObject json, QString url );

};

#endif // AGENTGENERATORDATASOURCE_H
