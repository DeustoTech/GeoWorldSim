#include "ImporterExporter.h"

#include <QDir>
#include <QFile>
#include <QDebug>

GSSImporterExporter::GSSImporterExporter(QString path , QString file_name , QString file_extension , QObject *parent) : QObject( parent ){
    this->directory_path = path;
    this->file_name = file_name;
    this->file_extension = file_extension;

    if( !this->directory_path.isEmpty() ){
        QDir dir( this->directory_path );
        if( !dir.exists() ){
            qWarning() << QString("Directory %1 not found").arg( this->directory_path );
            return;
        }

        /*if( !this->file_name.isEmpty() ){
            QFile file( dir.absoluteFilePath( this->file_name + "." + this->file_extension ) );
            if( !file.exists() ){
                Debugger::warning( this , Q_FUNC_INFO , QString("File %1 not found").arg( dir.absoluteFilePath( this->file_name + "." + this->file_extension ) ) );
                return;
            }
            file.close();
        }*/

    }
}

QString GSSImporterExporter::getPath() const{
    return this->directory_path;
}

QString GSSImporterExporter::getFilename() const{
    return this->file_name;
}

QString GSSImporterExporter::getFileExtension() const{
    return this->file_extension;
}

QString GSSImporterExporter::getAbsoluteFilePath() const{
    QDir dir( this->directory_path );
    return dir.absoluteFilePath( this->file_name + ( this->file_extension.isEmpty() ? "" : QString(".%1").arg( this->file_extension ) ) );
}
