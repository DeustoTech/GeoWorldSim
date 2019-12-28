#include "ShpImporter.h"
#include <QDebug>
#include <QDir>

#include "environment/Environment.h"
#include "utils/geometry/GeometryFactory.h"
#include "utils/conversors/projection/ProjectionConversor.h"

/**
 * @brief ShpToGeometry::open Give the path and file name WITHOUT EXTENSION of the SHP file that wants to be openned.
 * All SHP, SHX, DBF must have the same name.
 * For example if the SHP, SHX, DBF are called territory.shp, territory.shx, territory.dbf pass as parameter
 * ("/user/home/folder/" , "territory")
 * @param filepath
 * @return
 */
ShpImporter::ShpImporter(QString path , QString filename , QObject* parent) : GSSImporterExporter( path , filename , "shp" , parent ), shp_handle(0), dbf_handle(0), shape_count(0), shape_type(0){

    QDir dir( this->directory_path );

    shp_handle = SHPOpen( dir.absoluteFilePath( this->file_name ).toStdString().c_str() , "rb");
    dbf_handle = DBFOpen( dir.absoluteFilePath( this->file_name ).toStdString().c_str() , "rb" );

    if ( !shp_handle && !dbf_handle ) {
        return;
    } else {
        SHPGetInfo(shp_handle, &shape_count, &shape_type, 0, 0);
    }
}

ShpImporter::~ShpImporter(){
    if ( this->shp_handle ) { SHPClose(shp_handle); }
    if ( this->dbf_handle ){ DBFClose(dbf_handle); }
}

/**********************************************************************
 METHODS
**********************************************************************/

int ShpImporter::getShapeCount(){
    return shape_count;
}

GWSGeometry* ShpImporter::getShape( GWSGeometryFactory* factory , int index , QJsonObject &properties , int file_epsg , int convert_epsg ){

    if ( !shp_handle ) {
        return 0;
    }

    SHPObject *obj = SHPReadObject( shp_handle, index );

    // Properties
    properties.insert("shape-id" , obj->nShapeId);
    if( dbf_handle ){
        for(int p = 0; p < DBFGetFieldCount( dbf_handle ); p++){

            char field_name[12];
            switch ( DBFGetFieldInfo( dbf_handle , p , field_name, 0, 0) ) {
            case FTString:
                properties.insert( QString( field_name ) , DBFReadStringAttribute( dbf_handle , obj->nShapeId , p ) );
                break;
            case FTInteger:
                properties.insert( QString( field_name ) , DBFReadIntegerAttribute( dbf_handle , obj->nShapeId , p ) );
                break;
            case FTDouble:
                properties.insert( QString( field_name ) , DBFReadDoubleAttribute( dbf_handle , obj->nShapeId , p ) );
                break;
            case FTLogical:
                properties.insert( QString( field_name ) , DBFReadLogicalAttribute( dbf_handle , obj->nShapeId , p ) );
                break;
            default:
                break;
            }
        }
    }

    // Geometry
    GWSGeometry* geom = this->parseShape( factory , obj , file_epsg , convert_epsg );
    SHPDestroyObject( obj );
    return geom;
}


/**********************************************************************
 PRIVATES
**********************************************************************/

GWSGeometry* ShpImporter::parseShape(GWSGeometryFactory* factory , SHPObject *obj , int file_epsg , int convert_epsg){

    QList< QList<GWSCoordinate> > sequences;

    if( obj->nParts == 0 ){ //# of Parts (0 implies single part with no info)

        double x = obj->dfXMax;
        double y = obj->dfYMax;
        double z = obj->dfZMax;

        QList<GWSCoordinate> coordinates;
        if( file_epsg != convert_epsg ){
            coordinates.append( ProjectionConversor::getInstance()->convert( x , y , z , file_epsg , convert_epsg ) );
        } else {
            coordinates.append( GWSCoordinate( x , y , z ) );
        }
        sequences.append( coordinates );

    } else {

        for(int part = 0; part < obj->nParts; part++){

            QList<GWSCoordinate> coordinates;
            int part_vertices = 0;

            if (part == obj->nParts - 1) {
                // Last (or unique) part
                part_vertices = obj->nVertices - obj->panPartStart[part];
            } else {
                part_vertices = obj->panPartStart[part + 1] - obj->panPartStart[part];
            }

            for(int vertex = obj->panPartStart[part]; vertex < obj->panPartStart[part] + part_vertices; vertex++){

                double x = obj->padfX[vertex];
                double y = obj->padfY[vertex];
                double z = obj->padfZ[vertex];

                if( file_epsg != convert_epsg ){
                    coordinates.append( ProjectionConversor::getInstance()->convert( x , y , z , file_epsg , convert_epsg ) );
                } else {
                    coordinates.append( GWSCoordinate( x , y , z ) );
                }

            }
            sequences.append( coordinates );
        }
    }

    if( sequences.isEmpty() ){
        return 0;
    }

    switch( obj->nSHPType ){
    case SHPT_POINT:
    case SHPT_POINTZ:
    case SHPT_POINTM:
        return factory->createPoint( sequences.at(0).at(0) );

    case SHPT_ARC:
    case SHPT_ARCZ:
    case SHPT_ARCM:
        return factory->createLineString( sequences.at(0) );

    case SHPT_POLYGON:
    case SHPT_POLYGONZ:
    case SHPT_POLYGONM: {

        // Check if first cooridnate of ring is also last, otherwise add it
        foreach(QList<GWSCoordinate> coordinates , sequences){
            if( coordinates.first() != coordinates.last() ){
                coordinates.append( coordinates.first() );
            }
        }
        return factory->createPolygon( sequences );
    }

    case SHPT_MULTIPOINT:
    case SHPT_MULTIPOINTZ:
    case SHPT_MULTIPOINTM:

        break;

    default:
        break;
    }

    return 0;
}
