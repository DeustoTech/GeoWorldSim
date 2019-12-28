#ifndef SHPIMPORTER_H
#define SHPIMPORTER_H

#include "utils/io/ImporterExporter.h"
#include "shapefil.h"

#include "utils/geometry/Geometry.h"

class ShpImporter : public GSSImporterExporter
{
    Q_OBJECT
public:

    explicit ShpImporter(QString directory_path , QString filename, QObject* parent = 0);
    ~ShpImporter();

    // GETTERS
    int getShapeCount();
    GWSGeometry* getShape(GWSGeometryFactory* factory , int index , QJsonObject &properties , int file_epsg = 0 , int convert_epsg = 0 );

private:

    GWSGeometry* parseShape(GWSGeometryFactory* factory , SHPObject *obj , int file_epsg = 0 , int convert_epsg = 0 );

    SHPHandle shp_handle;
    DBFHandle dbf_handle;
    int shape_count;
    int shape_type;

};

#endif // SHPIMPORTER_H
