#ifndef OSMIMPORTER_H
#define OSMIMPORTER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QXmlStreamReader>

#include "utils/io/ImporterExporter.h"
#include "utils/geometry/Geometry.h"

class OsmImporter : public GSSImporterExporter
{
    Q_OBJECT
public:

    // HELPER STRUCT
    struct OsmElement {
        QString id;
        QString type;
        QMap<QString, QString> tags;
        GWSGeometry* geometry;
    };

    explicit OsmImporter(QString directory_path , QString filename , QString file_extension = "osm" , QObject *parent = 0);

    // GETTERS
    QList<OsmElement> getOsmElements( bool nodes = true , bool ways = true , bool relations = true );

private:

    // METHODS
    OsmElement processNode( QXmlStreamReader* xml );
    OsmElement processWay( QXmlStreamReader* xml , QMap<QString , OsmImporter::OsmElement> nodes );
    OsmElement processRelation( QXmlStreamReader* xml , QMap<QString , OsmImporter::OsmElement> nodes , QMap<QString , OsmImporter::OsmElement> ways );
};

#endif // OSMIMPORTER_H
