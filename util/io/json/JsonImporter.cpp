#include "JsonImporter.h"

#include <QFile>
#include <QDir>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

JsonImporter::JsonImporter(QString path, QString filename , QObject *parent) : GSSImporterExporter( path , filename , "json" , parent ){
}

/**********************************************************************
 GETTERS
**********************************************************************/

QJsonObject JsonImporter::getJson(){

    QDir dir( this->directory_path );
    QFile file( dir.absoluteFilePath( this->file_name ) );

    QJsonObject json;
    QJsonParseError jerror;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QJsonDocument json_document = QJsonDocument::fromJson(file.readAll(), &jerror);
        if(jerror.error != QJsonParseError::NoError){
            qWarning() << QString("Error when parsing JSON file %1").arg( jerror.errorString() );
        } else {
            json = json_document.object();
        }
        file.close();
    }

    return json;
}

