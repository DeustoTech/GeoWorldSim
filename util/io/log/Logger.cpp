#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>

#include "Logger.h"

QString geoworldsim::io::GWSLogger::log_name = "log";

/**********************************************************************
 GETTERS
**********************************************************************/

QString geoworldsim::io::GWSLogger::getLogName(){
    return geoworldsim::io::GWSLogger::log_name;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::io::GWSLogger::setLogName(QString name){
    geoworldsim::io::GWSLogger::log_name = name;
}

/**********************************************************************
 METHODS
**********************************************************************/

void geoworldsim::io::GWSLogger::log(QVariant to_write, QString log_name){

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
       geoworldsim::io::GWSLogger::log( to_write , log_name );

   }
}

void geoworldsim::io::GWSLogger::log(QStringList list, QString log_name){
    foreach(QString s , list){
        geoworldsim::io::GWSLogger::log( s , log_name );
    }
}

/**********************************************************************
 PROTECTED
**********************************************************************/

QString geoworldsim::io::GWSLogger::getDomainPath(const QMetaObject agent_class){
    QString domain_path;

    const QMetaObject* obj = &agent_class;
    while( obj ){
        domain_path.prepend( QString("/%1").arg( obj->className() ) );
        obj = obj->superClass();
    }

    return domain_path;
}
