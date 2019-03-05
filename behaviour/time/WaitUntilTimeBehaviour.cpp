#include "WaitUntilTimeBehaviour.h"

#include <QDateTime>

#include "../../environment/time_environment/TimeEnvironment.h"


QString WaitUntilTimeBehaviour::WAIT_UNTIL_YEAR = "wait_until_year";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_MONTH = "wait_until_month";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_DAY = "wait_until_day";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_HOUR = "wait_until_hour";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_MINUTE = "wait_until_minute";
QString WaitUntilTimeBehaviour::WAIT_UNTIL_SECOND = "wait_until_second";

QString WaitUntilTimeBehaviour::NEXTS = "nexts";

WaitUntilTimeBehaviour::WaitUntilTimeBehaviour() : GWSBehaviour(){

}


QJsonArray WaitUntilTimeBehaviour::behave(){

    qint64 current_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getAgent() );
    QDateTime current_datetime = QDateTime::fromMSecsSinceEpoch( current_time );

    int year = this->getProperty( WAIT_UNTIL_YEAR ).toDouble( current_datetime.date().year() );
    year = this->getProperty( WAIT_UNTIL_YEAR ).toInt( year );

    int month = this->getProperty( WAIT_UNTIL_MONTH ).toDouble( current_datetime.date().month() );
    month = this->getProperty( WAIT_UNTIL_MONTH ).toInt( month );

    int day = this->getProperty( WAIT_UNTIL_DAY ).toDouble( current_datetime.date().day() );
    day = this->getProperty( WAIT_UNTIL_DAY ).toInt( day );

    int hour = this->getProperty( WAIT_UNTIL_HOUR ).toDouble( current_datetime.time().hour() );
    hour = this->getProperty( WAIT_UNTIL_HOUR ).toInt( hour );

    int minute = this->getProperty( WAIT_UNTIL_MINUTE ).toDouble( current_datetime.time().minute() );
    minute = this->getProperty( WAIT_UNTIL_MINUTE ).toInt( minute );

    int second = this->getProperty( WAIT_UNTIL_SECOND ).toDouble( current_datetime.time().second() );
    second = this->getProperty( WAIT_UNTIL_SECOND ).toInt( second );

    QDate wait_until_day = QDate( year, month, day);
    QTime wait_until_time = QTime( hour , minute , second );
    QDateTime wait_until = QDateTime( current_datetime );
    wait_until.setDate( wait_until_day );
    wait_until.setTime( wait_until_time );

    // Wait until DAY has already past, so wait until next day's TIME
    if( wait_until < current_datetime ){
        wait_until = wait_until.addYears( 1 );
    }

    int seconds = current_datetime.secsTo( wait_until ); // IN seconds
    this->setProperty( GWSBehaviour::BEHAVIOUR_DURATION , seconds );

    return this->getProperty( NEXTS ).toArray();
}
