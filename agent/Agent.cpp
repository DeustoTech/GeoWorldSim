#include "Agent.h"

#include <QMetaProperty>
#include <QSqlField>
#include <QJsonValue>
#include <QJsonDocument>
#include <QPainter>
#include <QThread>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../behaviour/Behaviour.h"
#include "../../skill/Skill.h"

#include "../../environment/Environment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSAgent::STYLE_PROP = "style";

GWSAgent::GWSAgent( QObject* parent ) : GWSObject( parent ) , GWSStyle( this ) , busy_counter(0) {

}

GWSAgent::~GWSAgent() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , agent , &Agent::deleteLater );

    foreach (GWSEnvironment* env , this->environments_registerd_in ) {
        env->unregisterAgent( this );
    }

    // Emit withoug 'geo' property to be removed
    emit GWSApp::globalInstance()->pushAgentSignal( this->serializeMini() );

    qDebug() << QString("%1:%2 deleted").arg( this->metaObject()->className() ).arg( this->getId() );
    if( this->timer ){ this->timer->deleteLater(); }
    if( this->skills ){ this->skills->deleteAll(); this->skills->deleteLater(); }
    if( this->behaviours ){ this->behaviours->deleteAll(); this->behaviours->deleteLater(); }

}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgent::deserialize(QJsonObject json){

    GWSObject::deserialize( json );

    // Add to environments
    GWSEnvironment::globalInstance()->registerAgent( this );

    // SKILLS
    if( this->skills ){
        this->skills->deleteAll();
        this->skills->deleteLater();
    }
    QJsonArray jskills = json.value("@skills").toArray();
    foreach( QJsonValue js , jskills ){
        GWSSkill* skill = dynamic_cast<GWSSkill*>( GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this ) );
        if( !skill ){ continue; }
        this->addSkill( skill );
    }

    // BEHAVIOURS
    if( this->behaviours ){
        this->behaviours->deleteAll();
        this->behaviours->deleteLater();
    }
    QJsonArray jsbehaviours = json.value("@behaviours").toArray();
    foreach( QJsonValue js , jsbehaviours ){
        GWSBehaviour* behaviour = dynamic_cast<GWSBehaviour*>( GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this ) );
        if( !behaviour ){ continue; }
        this->addBehaviour( behaviour );
    }

    // INTERNAL TIME
    if( json.keys().contains( GWSTimeEnvironment::INTERNAL_TIME_PROP ) ){
        GWSTimeEnvironment::globalInstance()->registerAgent( this , json.value( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble() );
    } else {
        GWSTimeEnvironment::globalInstance()->unregisterAgent( this );
    }

    // GEOMETRY (comes parsed by GWSObject, extract and set it to null)
    if( GWSGeometry* geom = this->getProperty( GWSPhysicalEnvironment::GEOMETRY_PROP ).value<GWSGeometry*>() ){
        GWSPhysicalEnvironment::globalInstance()->registerAgent( this , geom );
        this->setProperty( GWSPhysicalEnvironment::GEOMETRY_PROP , QVariant() );
    } else {
        GWSPhysicalEnvironment::globalInstance()->unregisterAgent( this );
    }

    // STYLE
    if( !json.value( STYLE_PROP ).isNull() ){
        GWSStyle::deserialize( json.value( STYLE_PROP ).toObject() );
    }

    // RUNNING
    if( json.keys().contains( GWSExecutionEnvironment::RUNNING_PROP ) ){
        GWSExecutionEnvironment::globalInstance()->registerAgent( this );
    } else {
        GWSExecutionEnvironment::globalInstance()->unregisterAgent( this );
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
            skills.append( s->serialize() );
        }
    }
    json.insert( "@skills" , skills );

    // BEHAVIOUR
    QJsonArray behaviours;
    if( this->behaviours ){
        foreach (GWSObject* s , this->behaviours->getByClass( GWSBehaviour::staticMetaObject.className() ) ){
            behaviours.append( s->serialize() );
        }
    }
    json.insert( "@behaviours" , behaviours );

    // INTERNAL TIME
    json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this ) );

    // GEOMETRY
    const GWSGeometry* geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this );
    if( geom ){
        json.insert( GWSPhysicalEnvironment::GEOMETRY_PROP , geom->serialize() );
    }

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

bool GWSAgent::isBusy() const{
    return busy_counter > 0;
}

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

GWSBehaviour* GWSAgent::getBehaviour(QString id) const{
    return dynamic_cast<GWSBehaviour*>( this->behaviours->getByClassAndId( GWSBehaviour::staticMetaObject.className() , id ) );
}

QList<GWSBehaviour*> GWSAgent::getBehaviours(QString class_name) const{
    //return this->behaviours->getByClass<GWSBehaviour>( class_name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

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

void GWSAgent::addBehaviour(GWSBehaviour* behaviour){
    if( !this->behaviours ){
        this->behaviours = new GWSObjectStorage( this );
    }
    this->behaviours->add( behaviour );
}

void GWSAgent::setStartBehaviour(GWSBehaviour *behaviour){
    this->start_behaviour = behaviour;
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the agents thread.
 **/
void GWSAgent::tick(){

    emit GWSApp::globalInstance()->pushAgentSignal( this->serialize() );

    this->incrementBusy();
    this->behave();
    this->decrementBusy();

    emit this->agentBehavedSignal();
}

void GWSAgent::behave(){

    // No start behaviour
    if( !this->start_behaviour ){
        qWarning() << QString("Agent %1 %2 has no start behaviour. If running, it will probablly block execution time wating for it.").arg( this->metaObject()->className() ).arg( this->getId() );
        return;
    }

    // First behaviour
    QList<GWSBehaviour*> checked_behaviours; // TODO check infinite loops
    GWSBehaviour* next_execute_behaviour = this->start_behaviour;

    while( next_execute_behaviour && next_execute_behaviour->finished() ){
            next_execute_behaviour = next_execute_behaviour->getNext();
    }

    if( next_execute_behaviour ){

        qDebug() << QString("Executing behaviour %1 %2").arg( next_execute_behaviour->metaObject()->className() ).arg( next_execute_behaviour->getId() );
        this->timer->singleShot( 10 + (qrand() % 100) , [this , next_execute_behaviour](){
            qint64 start_internal_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this );
            next_execute_behaviour->tick( start_internal_time );
        });
    }
}
