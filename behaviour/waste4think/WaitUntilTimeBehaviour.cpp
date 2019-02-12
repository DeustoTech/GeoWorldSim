#include "WaitUntilTimeBehaviour.h"

#include <QDateTime>

#include "../../environment/time_environment/TimeEnvironment.h"

QString WaitUntilTimeBehaviour::WAIT_UNTIL_HOUR = "wait_until_hour";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_MINUTE = "wait_until_minute";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_SECOND = "wait_until_second";
QString WaitUntilTimeBehaviour::NEXTS = "nexts";

WaitUntilTimeBehaviour::WaitUntilTimeBehaviour() : GWSBehaviour(){

}


QJsonArray WaitUntilTimeBehaviour::behave(){

    qint64 current_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getAgent() );
    QDateTime current_datetime = QDateTime::fromMSecsSinceEpoch( current_time );

    int hour = this->getProperty( WAIT_UNTIL_HOUR ).toDouble( current_datetime.time().hour() );
    hour = this->getProperty( WAIT_UNTIL_HOUR ).toInt( hour );

    int minute = this->getProperty( WAIT_UNTIL_MINUTE ).toDouble( current_datetime.time().minute() );
    minute = this->getProperty( WAIT_UNTIL_MINUTE ).toInt( minute );

    int second = this->getProperty( WAIT_UNTIL_SECOND ).toDouble( current_datetime.time().second() );
    second = this->getProperty( WAIT_UNTIL_SECOND ).toInt( second );

    QTime wait_until_time = QTime( hour , minute , second );
    QDateTime wait_until = QDateTime( current_datetime );
    wait_until.setTime( wait_until_time );

    // Wait until DAY has already past, so wait until next day's TIME
    if( wait_until < current_datetime ){
        wait_until = wait_until.addDays( 1 );
    }

    int seconds = current_datetime.secsTo( wait_until ); // IN seconds
    this->setProperty( GWSBehaviour::BEHAVIOUR_DURATION , seconds );

    return this->getProperty( NEXTS ).toArray();
}
