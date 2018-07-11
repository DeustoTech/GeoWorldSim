#include "Agent.h"

#include <QMetaProperty>
#include <QSqlField>
#include <QJsonValue>
#include <QJsonDocument>
#include <QPainter>
#include <QThread>

#include "../../environment/Environment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

#include "../../util/conversors/geojson_to_geometry/GeoJsonToGeometryConversor.h"
#include "../../util/image_downloader/ImageDownloader.h"

#include "../../behaviour/Behaviour.h"
#include "../../skill/Skill.h"

QString GWSAgent::RUNNING_PROP = "running";
QString GWSAgent::NEXT_TICK_PROP = "next_tick";

GWSAgent::GWSAgent( GWSObject* parent ) : GWSObject( parent ) , busy_counter(0) {
    this->style = new GWSUiStyle( this );
}

GWSAgent::~GWSAgent() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , agent , &Agent::deleteLater );

    QString("%1:%2 deleted").arg( this->metaObject()->className() ).arg( this->getId() );
    GWSEnvironment::globalInstance()->unregisterAgent( this );

    if( this->style ){ this->style->deleteLater(); }
    //if( this->geometry ){ this->geometry->deleteLater(); }
    if( this->timer ){ this->timer->deleteLater(); }

    qDeleteAll( this->skills->getByClass( QObject::staticMetaObject ) );
    this->skills->deleteLater();
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgent::deserialize(QJsonObject json){

    GWSObject::deserialize( json );

    // Set skills
    QJsonArray skills = json.value("@skills").toArray();
    foreach( QJsonValue skill , skills ){
        qDebug() << "SKILL" << skill;
    }
}

/**********************************************************************
 EXPOERTERS
**********************************************************************/

/**
 * @brief Agent::toJSON Called when asked for more info about this agent.
 * Contains all the additional information of the agent.
 * Can be overwritten if want to add extra variables to child classes
 * @return
 */
QJsonObject GWSAgent::serialize() const{

    QJsonObject json = GWSObject::serialize();
    QJsonArray skills;
    if( this->skills ){
        foreach (GWSObject* s , this->skills->getByClass( GWSSkill::staticMetaObject ) ){
            skills.append( s->serializeMini() );
        }
    }
    json.insert( "@skills" , skills );
    json.insert( "@busy" , this->busy_counter );
    return json;
}

/**
 * @brief Agent::toImage
 * @return
 */
/*QImage GWSAgent::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height ){
    return this->geometry->toImage( image_bounds , image_width , image_height , this->getStyle()->border_color , this->getStyle()->border_weight , this->getStyle()->color , this->getStyle()->icon );
}*/


/**********************************************************************
 GETTERS
**********************************************************************/

bool GWSAgent::isRunning() const{
    return this->property( GWSAgent::RUNNING_PROP ).toBool();
}

bool GWSAgent::isBusy() const{
    return busy_counter > 0;
}

qint64 GWSAgent::getNextTick() const{
    return this->property( GWSAgent::NEXT_TICK_PROP ).value<qint64>();
}

/*const GWSGeometry* GWSAgent::getGeometry() const{
    return this->geometry;
}*/

/**
 * @brief Agent::getCoordinate
 * Representative Coordinate of this agents location, USED FOR GRAPHS AND ROUTING
 * @return
 */
/*GWSCoordinate GWSAgent::getRepresentativeCoordinate() const{
    if( this->geometry ){
        return this->geometry->getRepresentativeCoordinate();
    }
    return GWSCoordinate(0,0,0);
}*/

GWSUiStyle* GWSAgent::getStyle() const{
    return this->style;
}

bool GWSAgent::hasSkill( QMetaObject metaobject ) const{
    return this->skills && this->skills->contains( metaobject );
}

GWSSkill* GWSAgent::getSkill( QMetaObject metaobject ) const{
    return this->getSkill<GWSSkill>( metaobject );
}

template <class T> T* GWSAgent::getSkill( QMetaObject metaobject ) const{
    if( !this->skills ){ return 0; }
    const QList<GWSObject*> objs = this->skills->getByClass( metaobject );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( metaobject.className() );
        return 0;
    }
    return dynamic_cast<T*>( objs.at(0) );
}

QList<GWSSkill*> GWSAgent::getSkills( QMetaObject metaobject ) const{
    return this->getSkills<GWSSkill>( metaobject );
}

template <class T> QList<T*> GWSAgent::getSkills( QMetaObject metaobject ) const{
    QList<GWSSkill*> s;
    if( !this->skills ){ return s; }
    foreach(QObject* obj , this->skills->getByClass( metaobject )){
        s.append( dynamic_cast<T*>( obj ) );
    }
    return s;
}

/**********************************************************************
 SETTERS
**********************************************************************/

/**
 * @brief GraphNode::setGeometry
 * WARNING, agent will take ownership of the geometry, Do not use
 * shared geometries, make a clone before passing it to agent!!
 * WARNING, Too much concurrency, always ensure the agent has a geometry
 * @param geom
 */
/*void Agent::setGeometry(GSSGeometry* new_geometry){

    if( this->geometry == new_geometry ){ return; }

    if( this->environment ){

        // Environment ensures geometry correction and index, etc.
        this->environment->setAgentGeometry( this , new_geometry );
        emit this->environment->pushToInterfaceSignal( this->toMiniJSON() , "UPDATE" );

    } else {

        // Can have whatever geometry
        this->mutex.lock();
        if( this->geometry ){
            this->geometry->deleteLater();
            this->geometry = 0;
        }
        if( new_geometry ){
            this->geometry = new_geometry->createCopy();
            new_geometry->deleteLater();
        }
        this->mutex.unlock();
    }
}*/

void GWSAgent::setNextTick( const qint64 datetime ){
    this->setProperty( GWSAgent::NEXT_TICK_PROP , datetime );
}

void GWSAgent::incrementNextTick( GWSTimeUnit seconds ){
    qint64 datetime = this->property( GWSAgent::NEXT_TICK_PROP ).value<qint64>();
    this->setProperty( GWSAgent::NEXT_TICK_PROP , datetime += ( qMax( 0.01 , seconds.number() ) * 1000 ) );  // Min 10 milliseconds
}

void GWSAgent::incrementBusy(){
    this->busy_counter++;
}

void GWSAgent::decrementBusy(){
    this->busy_counter = qMax( this->busy_counter-1 , 0 );
}

void GWSAgent::addSkill(GWSSkill *skill){
    if( skill->parent() == 0 ){ skill->setParent( this ); }
    Q_ASSERT( skill->parent() == this );

    if( !this->skills ){
        this->skills = new GWSObjectStorage( this );
    }
    this->skills->add( skill );
}

void GWSAgent::removeSkill(GWSSkill *skill){
    this->skills->remove( skill );
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the agents thread.
 **/
void GWSAgent::tick(){

    if( !this->isRunning() ){
        qInfo() << "Agent is not running, skipping behaviour";
        GWSExecutionEnvironment::globalInstance()->stopAgent( this );
        return;
    }

    this->incrementBusy();

    // Behave
    GWSBehaviour* execute_behaviour = this->behaviour_start;
    while( execute_behaviour && execute_behaviour->finished() ){
        execute_behaviour = execute_behaviour->getNext();
    }

    if( execute_behaviour ){
        this->timer->singleShot( 10 + (qrand() % 100) , execute_behaviour , &GWSBehaviour::tick );
    }

    this->decrementBusy();

    // No behaviour, not busy anymore
    if( !this->behaviour_start ){
        this->busy_counter = 0;
    }

    emit this->agentBehavedSignal();
}
