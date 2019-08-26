#ifndef GWSENTITYGENERATORDATASOURCE_H
#define GWSENTITYGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

#include "../../util/datasource/DatasourceReader.h"

class GWSEntityGeneratorDatasource : public QObject
{
    Q_OBJECT

public:
    GWSEntityGeneratorDatasource( QJsonObject configuration );
    GWSEntityGeneratorDatasource( QJsonObject entity_template , QString user_id , QString scenario_id , QString entity_type , QString entity_filter , int amount = 9999999 );
    ~GWSEntityGeneratorDatasource();

signals:
    void dataReadingFinishedSignal();

private:
    GWSDatasourceReader* generateEntities( QJsonObject entity_template , QString user_id , QString scenario_id , QString entity_type , QString entity_filter , int amount = 9999999);
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};

#endif // GWSENTITYGENERATORDATASOURCE_H
