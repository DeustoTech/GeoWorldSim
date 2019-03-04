#include "App.h"

#include <QHostAddress>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QBuffer>
#include <QDateTime>
#include <QException>
#include <QString>


#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/grid_environment/GridEnvironment.h"
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/social_environment/SocialEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../util/parallelism/ParallelismController.h"
#include "../../util/io/log/Logger.h"

GWSApp* GWSApp::globalInstance(int argc, char *argv[]){
    // int &argc, char *argv[]
    static GWSApp instance(argc , argv);
    return &instance;
}

GWSApp::GWSApp(int argc, char* argv[]) : QCoreApplication( argc , argv ) , created_timestamp( QDateTime::currentMSecsSinceEpoch() ) {

    for( int i = 0 ; i < argc ; i++){
        QString arg = QString( argv[i] );
        QStringList splitted = arg.split('=');
        if( splitted.size() == 2 ){
            qDebug() << QString("Argument found : %1 = %2").arg( splitted.at(0) ).arg( splitted.at(1 ) );
            this->setProperty( splitted.at(0).toLatin1() , splitted.at(1) );
        }
    }

    Q_ASSERT( !this->getAppId().isEmpty() );
    Q_ASSERT( !this->getUserId().isEmpty() );

    // Redirect outputs to file
    if( this->property("console").toBool() ){
        qInstallMessageHandler( [](QtMsgType type, const QMessageLogContext &context, const QString &msg){
            if( type >= 2 ){
                GWSLogger::log( QString("[%1 - %2] %3").arg( QDateTime::currentDateTime().toString() ).arg( type ).arg( msg ) , QString("console.txt") ); Q_UNUSED( context );
            }
        } );
    }

    // Call Parallelism controller to take this as main thread
    GWSParallelismController::globalInstance();

    // Init random generators
    qsrand( QDateTime::currentDateTime().time().second() );

}

GWSApp::~GWSApp(){
    this->setProperty( "id" , QVariant() ); // Set to nu2ll
}

/**********************************************************************
 GETTERS
**********************************************************************/

QString GWSApp::getAppId(){
    return this->property( "id" ).toString();
}

QString GWSApp::getUserId(){
    return this->property( "user_id" ).toString();
}

/**********************************************************************
 METHODS
**********************************************************************/

int GWSApp::exec(){

    try {
        QCoreApplication::exec(); // Real exec()
    } catch(std::exception &e){
        this->exec();
    }

    return -1;
}
