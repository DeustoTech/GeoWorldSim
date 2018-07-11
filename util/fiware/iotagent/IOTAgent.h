#ifndef IOTAGENT_H
#define IOTAGENT_H

#include <QObject>
#include <QUrl>

#include "utils/fiware/orion/OrionAPIDriver.h"

class IOTAgent : public QObject
{
    Q_OBJECT

public:
    explicit IOTAgent(QString orion_url, int orion_port, int iotagent_server_listen_port = -1, QObject *parent = 0);
    ~IOTAgent();

    // GETTERS
    QList<QJsonObject> getEntities();
    void getEntityById( QString entity_id , QString entity_type );
    void getEntitiesByType( QString entity_type );

    // SETTERS
    void createEntity( QString domain_path , QJsonObject entity_json );
    void patchEntity( QString domain_path , QString entity_id , QString entity_type , QJsonObject patch_json );
    void deleteEntity( QString domain_path , QString entity_id, QString entity_type );
    void sendObservation(QString domain_path , QString entity_id , QString entity_type, QJsonObject patch_json);

    void createSubscription(QString description , QString entity_type , QString entity_id , QStringList condition_attributes , QStringList notify_attributes );
    void deleteSubscription(QString subscription_id);

signals:
    void notificationReceivedSignal( QString entity_type , QString entity_id , QJsonObject entity_attributes );

private:
    OrionAPIDriver* orion_api; // From IOTAgent to Orion (Up)

    QHttpServer* iotagent_server; // From Orion to IOTAgent (Down)
    int iotagent_server_listen_port;

};

#endif // IOTAGENT_H
