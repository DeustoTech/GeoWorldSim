#ifndef TIFFIMPORTER_H
#define TIFFIMPORTER_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QMap>

#include "gdal/gdal.h"
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"

#include "utils/io/ImporterExporter.h"
#include "utils/grid/Grid.h"

class TiffImporter : public GSSImporterExporter
{
    Q_OBJECT
public:
    explicit TiffImporter(QString directory_path , QString file_name , QObject *parent = 0);

    // GETTERS
    GWSGrid getValues();

};

#endif // TIFFIMPORTER_H
