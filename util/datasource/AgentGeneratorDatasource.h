#ifndef GWSAGENTGENERATORDATASOURCE_H
#define GWSAGENTGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class GWSAgentGeneratorDatasource : public QObject
{

public:
    GWSAgentGeneratorDatasource( QJsonObject json, QString url );

private:
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};

#endif // GWSAGENTGENERATORDATASOURCE_H
