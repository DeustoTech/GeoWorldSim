#ifndef ORIONMIDDLEWARE_H
#define ORIONMIDDLEWARE_H

#include <QObject>
#include <QUrl>

#include "utils/api/NoReplyAPIDriver.h"
#include "utils/api/SyncAPIDriver.h"
#include "agents/Agent.h"

class OrionAPIDriver : public QObject
{
    Q_OBJECT
public:
    OrionAPIDriver(QString host, int port, QObject *parent = 0);
    ~OrionAPIDriver();

    // GETTERS
    QString checkVersion();
    QList<QJsonObject> getEntities();
    QList<QJsonObject> getEntityById( QString entity_id, QString entity_type );
    QList<QJsonObject> getEntitiesByType( QString entities_type );

    // SETTERS
    void createEntity( QString domain_path , QJsonObject entity_json );
    void patchEntity( QString domain_path , QString entity_id , QString entity_type, QJsonObject patch_json );
    void deleteEntity( QString domain_path , QString entity_id, QString entity_type );
    void createSubscription(QJsonObject subscription_json);
    void deleteSubscription(QString subscription_id);

private:

    QString host;
    int port;
    QString version_endpoint = "/version";
    QString entities_endpoint = "/v2/entities";
    QString subscriptions_endpoint = "/v2/subscription";
    QString fiware_service = "PucSandBox";
    QString token;


};

#endif // ORIONMIDDLEWARE_H
