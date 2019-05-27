#ifndef GWSAGENTGENERATORDATASOURCE_H
#define GWSAGENTGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class GWSAgentGeneratorDatasource : public QObject
{
    Q_OBJECT

public:
    GWSAgentGeneratorDatasource( QJsonObject json , QString scenario_id , QString entities_type , QString entities_filter , int amount = 9999999 );

signals:
    void dataReadingFinishedSignal();

private:
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};

#endif // GWSAGENTGENERATORDATASOURCE_H
