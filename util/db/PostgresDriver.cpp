#include <QSqlQuery>
#include <QSqlError>
#include <QThread>
#include <QDebug>

#include "PostgresDriver.h"

PostgresDriver::PostgresDriver(QString url, int port, QString db_name, QString db_user, QString db_pass, QString connection_name, QObject* parent ) : GWSObject( parent )
{
    this->connection_name = connection_name;

    QString psql_driver = "QPSQL";

    if( !QSqlDatabase::drivers().contains( psql_driver ) ){
        qCritical() << QString("Driver %1 is not available. Options are %2").arg( psql_driver ).arg( this->database.drivers().join(',') );
    }

    this->database = QSqlDatabase::addDatabase(psql_driver , this->connection_name);
    this->database.setHostName( url );
    this->database.setPort( port );
    this->database.setDatabaseName( db_name );
    this->database.setUserName( db_user );
    this->database.setPassword( db_pass );

    while( !this->database.open() ){
        this->thread()->sleep( 2 ); // Wait 2 seconds
        qCritical() << QString("Unable to connect to DB %1 %2 %3 waiting... ").arg( this->database.hostName() ).arg( this->database.port() ).arg( this->database.databaseName() );
    }
}

PostgresDriver::~PostgresDriver(){
    this->database.close();
    this->database.removeDatabase( this->connection_name );
}

/**********************************************************************
 GETTERS
**********************************************************************/

QList<QSqlRecord> PostgresDriver::executeQuery( QString sql ){

    QList<QSqlRecord> list;

    if( this->database.isOpen() ){
        QSqlQuery query( this->database.database( this->connection_name ) );

        query.exec( sql );

        QSqlError error = query.lastError();
        if (error.type() != QSqlError::NoError){
            qWarning() << QString("%1 \n %2").arg( error.text() ).arg( sql );
        }

        while( query.next() ){
            list.append(query.record());
        }

    } else {
        qCritical() << "Could not execute query because database is closed";
    }

    return list;
}
