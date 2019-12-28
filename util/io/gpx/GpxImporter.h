#ifndef GPXIMPORTER_H
#define GPXIMPORTER_H

#include <QObject>

#include <QXmlStreamReader>
#include <QDateTime>

#include "utils/io/ImporterExporter.h"

struct GpxPoint {
    double x;
    double y;
    double z;
    QDateTime datetime;
};

class GpxImporter : public GSSImporterExporter
{
    Q_OBJECT
public:
    explicit GpxImporter(QString directory_path , QString file_name , QString extension , QObject *parent = 0);

    // GETTERS
    QList<GpxPoint> getGpxPoints();

private:

    QList<GpxPoint> processPoints( QXmlStreamReader* xml );
    GpxPoint processPoint( QXmlStreamReader* xml );

};

#endif // GPXIMPORTER_H
