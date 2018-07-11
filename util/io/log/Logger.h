#ifndef GWSLOGGER_H
#define GWSLOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class GWSLogger : public QObject
{
    Q_OBJECT
public:

    // GETTERS
    static QString getLogName();

    // SETTERS
    static void setLogName(QString log_name);

    // METHODS
    static void log(QVariant to_write, QString log_name = GWSLogger::log_name );
    static void log(QStringList list, QString log_name = GWSLogger::log_name );

private:
    // METHODS
    static QString getDomainPath(const QMetaObject agent_class);

    static QString log_name;
};

#endif // GWSLOGGER_H

