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

    QTime wait_until_time = QTime(
                this->getProperty( WAIT_UNTIL_HOUR ).toInt( current_datetime.time().hour() ),
                this->getProperty( WAIT_UNTIL_MINUTE ).toInt( current_datetime.time().minute() ),
                this->getProperty( WAIT_UNTIL_SECOND ).toInt( current_datetime.time().second() )
                );
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
