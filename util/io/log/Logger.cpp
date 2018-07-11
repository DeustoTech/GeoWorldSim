#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>

#include "Logger.h"

QString GWSLogger::log_name = "log";

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSLogger::getLogName(){
    return GWSLogger::log_name;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSLogger::setLogName(QString name){
    GWSLogger::log_name = name;
}

/**********************************************************************
 METHODS
**********************************************************************/

void GWSLogger::log(QVariant to_write, QString log_name){

   QFile logfile( QDir::currentPath() + "/" + log_name );

   if( logfile.open( QIODevice::Append ) ){

       QTextStream out(&logfile);
       out << to_write.toString() << "\n";
       logfile.close();

   } else {

       bool created = false;

       // Check if file exists
       if( !logfile.exists() ){
           logfile.open( QIODevice::WriteOnly );
           logfile.close();
           created = true;
       }

       // Now try again
       Q_ASSERT( created );
       GWSLogger::log( to_write , log_name );

   }
}

void GWSLogger::log(QStringList list, QString log_name){
    foreach(QString s , list){
        GWSLogger::log( s , log_name );
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

QString GWSLogger::getDomainPath(const QMetaObject agent_class){
    QString domain_path;

    const QMetaObject* obj = &agent_class;
    while( obj ){
        domain_path.prepend( QString("/%1").arg( obj->className() ) );
        obj = obj->superClass();
    }

    return domain_path;
}
