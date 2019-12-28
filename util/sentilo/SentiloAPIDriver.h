#ifndef SENTILOAPIDRIVER_H
#define SENTILOAPIDRIVER_H

#include <QObject>
#include "QJsonObject"

class SentiloAPIDriver : public QObject
{
    Q_OBJECT

public:
    SentiloAPIDriver(QString host, int port, QObject *parent = 0 );
    ~SentiloAPIDriver();

    void getCatalog();

    void createEntity(QJsonObject entity_json);
    void patchEntity(QJsonObject patch_json);
    void deleteEntity(QJsonObject entity_json);

    void sendObservation( QString sensor_id, QJsonObject entity_json );


private:
    QString host;
    int port;

    // Endpoint and token provided for the simulation
    QString provider_endpoint = "/sandbox";
    QString token = "9840bb6c0f332a69362b9d41bbd5be1b8f478135b2b70435091f0993a06b38e4";

    QString catalog_endpoint = "/catalog";
    QString data_endpoint = "/data";
};

#endif // SENTILOAPIDRIVER_H
