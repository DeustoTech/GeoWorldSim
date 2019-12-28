#ifndef POSTGRESDRIVER_H
#define POSTGRESDRIVER_H

#include "utils/object/Object.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlRecord>

class PostgresDriver : public GWSObject
{
public:
    PostgresDriver(QString url, int port, QString db_name, QString db_user, QString db_pass, QString connection_name = "shared_connection" , QObject* parent = 0 );
    ~PostgresDriver();

    // GETERS
    QList<QSqlRecord> executeQuery(QString sql);

private:
    QSqlDatabase database;
    QString connection_name;
};

#endif // POSTGRESDRIVER_H
