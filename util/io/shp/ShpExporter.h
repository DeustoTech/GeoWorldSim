#ifndef SHPEXPORTER_H
#define SHPEXPORTER_H

#include "utils/io/ImporterExporter.h"

#include "shapefil.h"
#include "agents/Agent.h"
#include "utils/geometry/Geometry.h"

class ShpExporter : public QObject
{
    Q_OBJECT
public:

    explicit ShpExporter(QObject *parent = 0);
    ~ShpExporter();

    bool exportAgents(QList<GWSAgent*> agents, QString filename, QString geometry_type = "Polygon");

private:
    SHPHandle shp_handle;
    DBFHandle dbf_handle;

    static QString shp_folder_name;
};

#endif // SHPEXPORTER_H
