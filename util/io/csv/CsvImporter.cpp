#include "CsvImporter.h"
#include <QDebug>
#include <QDir>

#include <QFile>
#include <QTextStream>

CsvImporter::CsvImporter( QString path , QString filename , QString extension , char separator, QObject *parent) : GSSImporterExporter( path , filename , extension , parent ){
    this->separator = separator;
    this->getColumnNames();
}

/**********************************************************************
 GETTERS
**********************************************************************/

QMap<QString, int> CsvImporter::getColumnNames(){

    if( !this->column_names.isEmpty() ){
        return this->column_names;
    }

    QFile file( this->getAbsoluteFilePath() );
    QTextStream in( &file );

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QList<QString> header = in.readLine().split(this->separator);
        for ( int i=0 ; i < header.size() ; i++ ) {
            this->column_names.insert( header.at(i).toUpper(), i );
        }
        file.close();
    }
    return this->column_names;
}


QList<QString> CsvImporter::getRows(){

    QFile file( this->getAbsoluteFilePath() );
    QTextStream in(&file);

    QList<QString> rows;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        in.readLine(); // Jump first line
        while (!in.atEnd()) {
            rows.append( in.readLine() );
        }
        file.close();
    }
    return rows;
}

QMap<QString,QString> CsvImporter::parseRow(QString row){
    QMap<QString , QString> m;
    QList<QString> row_fields = row.split(this->separator);
    foreach( QString column_name , this->column_names.keys() ){
        m.insert( column_name , row_fields.at( column_names.value( column_name ) ) );
    }
    return m;
}
