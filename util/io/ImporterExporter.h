#ifndef GSSIMPORTEREXPORTER_H
#define GSSIMPORTEREXPORTER_H

#include <QObject>

class GSSImporterExporter : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit GSSImporterExporter(QString directory_path , QString file_name = "", QString file_extension = "", QObject *parent = 0);

    // GETTERS
    QString getPath() const;
    QString getFilename() const;
    QString getFileExtension() const;
    QString getAbsoluteFilePath() const;

protected:
    QString directory_path; // Path of the directory to import
    QString file_name; // Filename at the path to import
    QString file_extension; // File extension
    QStringList extensions; // Extensions that can be imported
};

#endif // GSSIMPORTEREXPORTER_H
