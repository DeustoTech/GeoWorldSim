#include "WaitUntilTimeBehaviour.h"

#include <QDateTime>

#include "../../environment/time_environment/TimeEnvironment.h"

QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_YEAR = "wait_until_year";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_MONTH = "wait_until_month";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_DAY = "wait_until_day";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_HOUR = "wait_until_hour";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_MINUTE = "wait_until_minute";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_UNTIL_SECOND = "wait_until_second";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::WAIT_FREQUENCY = "wait_frequency";
QString geoworldsim::behaviour::WaitUntilTimeBehaviour::NEXTS = "nexts";

geoworldsim::behaviour::WaitUntilTimeBehaviour::WaitUntilTimeBehaviour() : Behaviour(){

}

QPair< double , QJsonArray > geoworldsim::behaviour::WaitUntilTimeBehaviour::behave(){

    QSharedPointer< Entity > entity = this->getEntity();
    qint64 current_time = entity->getProperty( environment::TimeEnvironment::INTERNAL_TIME_PROP ).toDouble();
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

    // Get periodicity of the wait
    // Wait until DAY has already past, so wait until next day's TIME
    QString wait_periodicity = this->getProperty( WAIT_FREQUENCY ).toString();

    if( wait_until < current_datetime ){
        if ( wait_periodicity == "year" ){
            wait_until = wait_until.addYears( 1 );
        }
        if ( wait_periodicity == "month" ){
            wait_until = wait_until.addMonths( 1 );
        }
        if ( wait_periodicity == "day" ){
            wait_until = wait_until.addDays( 1 );
        }
    }

    int seconds = current_datetime.secsTo( wait_until ) + 1; // IN seconds
    return QPair< double , QJsonArray >( seconds , this->getProperty( NEXTS ).toArray() );
}
