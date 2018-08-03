#include "Agent.h"

#include <QMetaProperty>
#include <QSqlField>
#include <QJsonValue>
#include <QJsonDocument>
#include <QPainter>
#include <QThread>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../environment/Environment.h"
#include "../../behaviour/Behaviour.h"
#include "../../skill/Skill.h"

QString GWSAgent::RUNNING_PROP = "running";
QString GWSAgent::GEOMETRY_PROP = "geo";
QString GWSAgent::STYLE_PROP = "style";
QString GWSAgent::INTERNAL_TIME_PROP = "internal_time";

GWSAgent::GWSAgent( QObject* parent ) : GWSObject( parent ) , GWSGeometry( this ) , GWSStyle( this ) , busy_counter(0) {
}

GWSAgent::~GWSAgent() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , agent , &Agent::deleteLater );

    foreach (GWSEnvironment* env , this->environments_registerd_in ) {
        env->unregisterAgent( this );
    }

    // Emit withoug 'geo' property to be removed
    emit GWSApp::globalInstance()->pushAgentSignal( this->serializeMini() );

    QString("%1:%2 deleted").arg( this->metaObject()->className() ).arg( this->getId() );
    if( this->timer ){ this->timer->deleteLater(); }

    this->skills->deleteLater();

}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgent::deserialize(QJsonObject json){

    GWSObject::deserialize( json );

    // SKILLS
    QJsonArray jskills = json.value("@skills").toArray();
    foreach( QJsonValue js , jskills ){
        GWSSkill* skill = dynamic_cast<GWSSkill*>( GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this ) );
        if( !skill ){ continue; }
        this->addSkill( skill );
    }

    // BEHAVIOURS
    QJsonArray jsbehaviours = json.value("@behaviours").toArray();
    foreach( QJsonValue js , jsbehaviours ){
        GWSBehaviour* behaviour = dynamic_cast<GWSBehaviour*>( GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this ) );
        if( !behaviour ){ continue; }
        this->addBehaviour( behaviour );
    }

    // GEOMETRY
    if( !json.value( GEOMETRY_PROP ).isNull() ){
        GWSGeometry::deserialize( json.value( GEOMETRY_PROP ).toObject() );
    }

    // STYLE
    if( !json.value( STYLE_PROP ).isNull() ){
        GWSStyle::deserialize( json.value( STYLE_PROP ).toObject() );
    }
}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/**
 * @brief Agent::toJSON Called when asked for more info about this agent.
 * Contains all the additional information of the agent.
 * Can be overwritten if want to add extra variables to child classes
 * @return
 */
QJsonObject GWSAgent::serialize() const{

    QJsonObject json = GWSObject::serialize();

    //Skills
    QJsonArray skills;
    if( this->skills ){
        foreach (GWSObject* s , this->skills->getByClass( GWSSkill::staticMetaObject.className() ) ){
            skills.append( s->serializeMini() );
        }
    }
    json.insert( "@skills" , skills );

    // BEHAVIOUR

    // GEOMETRY
    json.insert( GEOMETRY_PROP , GWSGeometry::serialize() );

    // STYLE
    json.insert( STYLE_PROP , GWSStyle::serialize() );

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

QList<GWSEnvironment*> GWSAgent::getEnvironments() const{
    return this->environments_registerd_in;
}

bool GWSAgent::isRunning() const{
    return this->getProperty( GWSAgent::RUNNING_PROP ).toBool();
}

bool GWSAgent::isBusy() const{
    return busy_counter > 0;
}

qint64 GWSAgent::getInternalTime() const{
    return this->getProperty( GWSAgent::INTERNAL_TIME_PROP ).value<qint64>();
}

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

bool GWSAgent::hasSkill( QString class_name ) const{
    return this->skills && this->skills->contains( class_name );
}

GWSSkill* GWSAgent::getSkill( QString class_name ) const{
    return this->getSkill<GWSSkill>( class_name );
}

template <class T> T* GWSAgent::getSkill( QString class_name ) const{
    if( !this->skills ){ return 0; }
    const QList<GWSObject*> objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( class_name );
        return 0;
    }
    return dynamic_cast<T*>( objs.at(0) );
}

QList<GWSSkill*> GWSAgent::getSkills( QString class_name ) const{
    return this->getSkills<GWSSkill>( class_name );
}

template <class T> QList<T*> GWSAgent::getSkills( QString class_name ) const{
    QList<GWSSkill*> s;
    if( !this->skills ){ return s; }
    foreach(QObject* obj , this->skills->getByClass( class_name )){
        s.append( dynamic_cast<T*>( obj ) );
    }
    return s;
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSAgent::addBehaviour(GWSBehaviour* behaviour){
    this->behaviours.append( behaviour );
}

void GWSAgent::setInternalTime( const qint64 datetime ){
    this->setProperty( GWSAgent::INTERNAL_TIME_PROP , datetime );
}

void GWSAgent::incrementInternalTime( GWSTimeUnit seconds ){
    qint64 datetime = this->getProperty( GWSAgent::INTERNAL_TIME_PROP ).value<qint64>();
    this->setProperty( GWSAgent::INTERNAL_TIME_PROP , datetime += ( qMax( 0.01 , seconds.number() ) * 1000 ) );  // Min 10 milliseconds
}

void GWSAgent::incrementBusy(){
    this->busy_counter++;
}

void GWSAgent::decrementBusy(){
    this->busy_counter = qMax( this->busy_counter-1 , 0 );
}

void GWSAgent::addSkill(GWSSkill *skill){
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
        return;
    }

    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );

    this->incrementBusy();
    this->behave();
    this->decrementBusy();

    emit this->agentBehavedSignal();
}

void GWSAgent::behave(){

    // No behaviours
    if( this->behaviours.isEmpty() ){
        return;
    }

    GWSBehaviour* next_execute_behaviour = Q_NULLPTR;
    foreach (GWSBehaviour* b , this->behaviours) {
        if( !b->finished() ){
            next_execute_behaviour = b;
            break;
        }
    }

    if( next_execute_behaviour ){
        qDebug() << QString("Executing behaviour %1 %2").arg( next_execute_behaviour->metaObject()->className() ).arg( next_execute_behaviour->getId() );
        this->timer->singleShot( 10 + (qrand() % 100) , next_execute_behaviour , &GWSBehaviour::tick );
    }
}
