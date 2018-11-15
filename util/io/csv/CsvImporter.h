#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>

#include "../../util/io/ImporterExporter.h"

class CsvImporter : public GSSImporterExporter
{
    Q_OBJECT
public:
    explicit CsvImporter(QString directory_path , QString file_name , QString extension , char separator = ',', QObject *parent = 0);

    // GETTERS
    QMap<QString,int> getColumnNames();
    QList<QString> getRows();
    QMap<QString , QString> parseRow( QString row );

private:
    char separator;
    QMap<QString, int> column_names;

};

#endif // CSVIMPORTER_H
