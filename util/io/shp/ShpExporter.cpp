#include "ShpExporter.h"

#include <QDir>

QString ShpExporter::shp_folder_name = "shps";

ShpExporter::ShpExporter(QObject *parent) : QObject(parent){

    // Check if common_path folder exists
    if( !QDir( QDir::currentPath() + "/" + ShpExporter::shp_folder_name ).exists() ){
        qDebug() << QString("Creating shps folder in %1/%2").arg( QDir::currentPath() ).arg( shp_folder_name );
        QDir().mkdir( QDir::currentPath() + "/" + ShpExporter::shp_folder_name );
    }

    this->shp_handle = 0;
    this->dbf_handle = 0;
}

ShpExporter::~ShpExporter(){
    if ( this->shp_handle ) {
        SHPClose(shp_handle);
    }
    if ( this->dbf_handle ){
        DBFClose(dbf_handle);
    }
}

bool ShpExporter::exportAgents(QList<GWSAgent *> agents, QString filename, QString geometry_type){

    bool completed = true;

    if( agents.isEmpty() ){
        return completed;
    }

    QMap<QString, int> attr_name_pos;
    QMap<QString, QString> attr_name_type;

    int g_type = -1;
    if( geometry_type == "Point" ){
        g_type =  SHPT_POINTZ;
    }
    if( geometry_type == "LineString" ){
        g_type =  SHPT_ARCZ;
    }
    if( geometry_type == "Polygon" ){
        g_type =  SHPT_POLYGONZ;
    }

    this->shp_handle = SHPCreate( (QDir::currentPath() + "/" + ShpExporter::shp_folder_name + "/" + filename).toStdString().c_str(), g_type );
    this->dbf_handle = DBFCreate( (QDir::currentPath() + "/" + ShpExporter::shp_folder_name + "/" + filename).toStdString().c_str() );

    for(int a = 0; a < agents.size(); a++){

        double* x = new double[ agents.at(a)->getGeometry()->getCoordinates().size() ];
        double* y = new double[ agents.at(a)->getGeometry()->getCoordinates().size() ];
        double* z = new double[ agents.at(a)->getGeometry()->getCoordinates().size() ];

        QList<GWSCoordinate> coordinates = agents.at(a)->getGeometry()->getCoordinates();
        for(int i = 0; i < coordinates.size(); i++){
            x[i] = coordinates.at(i).getX();
            y[i] = coordinates.at(i).getY();
            z[i] = coordinates.at(i).getZ();
        }

        SHPObject* obj = SHPCreateObject(g_type , a , 0 , NULL , NULL , coordinates.size() , x , y , z , NULL);
        SHPWriteObject( this->shp_handle , -1 , obj);
        SHPDestroyObject( obj );

        QJsonObject properties = agents.at(a)->serialize();
        foreach( QString key , properties.keys() ){

            int pos = attr_name_pos.contains(key) ? attr_name_pos.value(key) : -1;
            if( pos == -1 ){

                if( properties[key].isDouble() ){
                    pos = DBFAddField( this->dbf_handle , key.toStdString().c_str() , FTDouble, 10 , 10);
                    attr_name_type.insert( key , "double" );
                } else {
                    pos = DBFAddField( this->dbf_handle , key.toStdString().c_str() , FTString , 64 , 0);
                    attr_name_type.insert( key , "string" );
                }

                if( pos > -1 ){
                    attr_name_pos.insert( key , pos );
                } else {
                    qWarning() << QString("Could not create DBF field %1").arg( key );
                }
            }

            if( pos > -1 ){
                if( attr_name_type[key] == "double" ){
                    DBFWriteDoubleAttribute( this->dbf_handle , a , pos , properties[key].toDouble() );
                } else {
                    DBFWriteStringAttribute( this->dbf_handle , a , pos , properties[key].toString().toStdString().c_str() );
                }
            }
        }
    }

    return completed;
}
