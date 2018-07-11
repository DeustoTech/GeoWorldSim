#include "GpxImporter.h"

#include <QFile>
#include <QDir>
#include "environment/Environment.h"

GpxImporter::GpxImporter(QString path , QString file_name , QString extension , QObject *parent) : GSSImporterExporter( path , file_name , extension , parent ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<GpxPoint> GpxImporter::getGpxPoints(){
    QList<GpxPoint> gpx_points;

    QDir dir( this->directory_path );
    QFile file( dir.absoluteFilePath( this->file_name ) );

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){

        QXmlStreamReader* xml = new QXmlStreamReader( &file );

        while( !xml->atEnd() ){
            xml->readNextStartElement();
            if ( xml->name() == "trkseg" ){
                gpx_points.append( this->processPoints( xml ) );
            }
        }
        if ( xml->hasError() ) {
            qWarning() << QString("Error parsing GPX XML").arg( xml->errorString() );
        }

        delete xml;
        file.close();
    }

    return gpx_points;
}

/**********************************************************************
 METHODS
**********************************************************************/

QList<GpxPoint> GpxImporter::processPoints( QXmlStreamReader* xml ){
    QList<GpxPoint> ps;

    if ( !xml->isStartElement() || xml->name() != "trkseg" ){
        return ps;
    }

    while ( !xml->atEnd() ) {
        if ( xml->name() == "trkpt"){
            ps.append( this->processPoint( xml ) );
        } else {
            xml->readNextStartElement();
        }
    }

    return ps;
}


GpxPoint GpxImporter::processPoint( QXmlStreamReader* xml ){
    GpxPoint point;
    if ( !xml->isStartElement() || xml->name() != "trkpt"){
        return point;
    }

    point.x = xml->attributes().value("lon").toDouble();
    point.y = xml->attributes().value("lat").toDouble();
    point.z = 0;

    while ( xml->readNextStartElement() && xml->name() != "trkpt") {
        if ( xml->name() == "ele"){
            point.z = xml->readElementText().toDouble();
        }
        if ( xml->name() == "time") {
            point.datetime = QDateTime::fromString( xml->readElementText() );
        }
        xml->skipCurrentElement();
    }

    return point;
}
