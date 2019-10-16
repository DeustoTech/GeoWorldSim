#ifndef ENTITYGENERATORDATASOURCE_H
#define ENTITYGENERATORDATASOURCE_H

#include <QObject>
#include <QJsonObject>

#include "../../util/datasource/DatasourceReader.h"

namespace geoworldsim {
namespace datasource {


class EntityGeneratorDatasource : public QObject
{
    Q_OBJECT

public:
    EntityGeneratorDatasource( QJsonObject configuration );
    EntityGeneratorDatasource( QJsonObject entity_template , QString user_id , QString scenario_id , QString entity_type , QString entity_filter , int amount = 9999999 );
    ~EntityGeneratorDatasource();

signals:
    void dataReadingFinishedSignal();

private:
    DatasourceReader* generateEntities( QJsonObject entity_template , QString user_id , QString scenario_id , QString entity_type , QString entity_filter , int amount = 9999999);
    QJsonObject joinJSON( QJsonObject json_template , QJsonObject json_data );

};


}
}


#endif // ENTITYGENERATORDATASOURCE_H
