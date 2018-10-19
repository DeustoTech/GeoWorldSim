#ifndef AGENTGENERATORDATASOURCE_H
#define AGENTGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class AgentGeneratorDatasource : public QObject
{

public:
    AgentGeneratorDatasource( QJsonObject json, QString url );

private:
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};

#endif // AGENTGENERATORDATASOURCE_H
