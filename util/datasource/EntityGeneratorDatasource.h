#ifndef GWSENTITYGENERATORDATASOURCE_H
#define GWSENTITYGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

class GWSEntityGeneratorDatasource : public QObject
{
    Q_OBJECT

public:
    GWSEntityGeneratorDatasource( QJsonObject json , QString scenario_id , QString entity_type , QString entity_filter , int amount = 9999999 );

signals:
    void dataReadingFinishedSignal();

private:
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};

#endif // GWSENTITYGENERATORDATASOURCE_H
