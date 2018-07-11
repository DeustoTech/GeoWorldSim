#ifndef JSONIMPORTER_H
#define JSONIMPORTER_H

#include <QObject>
#include <QJsonObject>

#include "utils/io/ImporterExporter.h"

class JsonImporter : public GSSImporterExporter
{
    Q_OBJECT

public:
    explicit JsonImporter(QString directory_path , QString filename , QObject *parent = 0);

    // GETTERS
    QJsonObject getJson();
};

#endif // JSONIMPORTER_H
