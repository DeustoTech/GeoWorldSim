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
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSAgent::ALIVE_PROP = "alive";
QString GWSAgent::STYLE_PROP = "style";

GWSAgent::GWSAgent() : GWSObject() , GWSStyle( this ) , busy_counter(0) {
    this->setProperty( ALIVE_PROP , true );
}

GWSAgent::~GWSAgent() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , agent , &Agent::deleteLater );

    foreach(GWSEnvironment* env , this->environments_registerd_in ) {
        env->unregisterAgent( this->getSharedPointer() );
    }

    // Emit withoug 'ALIVE' property to be removed
    this->setProperty( ALIVE_PROP , false );
    emit GWSApp::globalInstance()->pushAgentSignal( this->serializeMini() );

    qDebug() << QString("%1 deleted").arg( this->getId() );
    if( this->timer ){ this->timer->deleteLater(); }
    if( this->skills ){ this->skills->deleteAll(); this->skills->deleteLater(); }
    if( this->behaviours ){ this->behaviours->deleteAll(); this->behaviours->deleteLater(); }

}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSAgent::deserialize(QJsonObject json , QSharedPointer<GWSObject> parent ){
    GWSObject::deserialize( json , parent );

    // SKILLS
    if( json.keys().contains( "@skills" ) ){
        if( this->skills ){
            this->skills->deleteAll();
            this->skills->deleteLater();
        }
        QJsonArray jskills = json.value("@skills").toArray();
        foreach( QJsonValue js , jskills ){
            QSharedPointer<GWSSkill> skill = GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this->getSharedPointer() ).dynamicCast<GWSSkill>();
            if( skill.isNull() ){ continue; }
            this->addSkill( skill );
        }
    }

    // BEHAVIOURS
    if( json.keys().contains( "@behaviours" ) ){
        if( this->behaviours ){
            this->behaviours->deleteAll();
            this->behaviours->deleteLater();
        }
        QJsonArray jsbehaviours = json.value("@behaviours").toArray();
        foreach( QJsonValue js , jsbehaviours ){
            QSharedPointer<GWSBehaviour> behaviour = GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this->getSharedPointer() ).dynamicCast<GWSBehaviour>();
            if( behaviour.isNull() ){ continue; }
            this->addBehaviour( behaviour );
        }
    }

    // STYLE
    if( !json.value( STYLE_PROP ).isNull() ){
        GWSStyle::deserialize( json.value( STYLE_PROP ).toObject() );
    }

    // RUNNING
    if( json.keys().contains( GWSExecutionEnvironment::RUNNING_PROP ) ){
        GWSExecutionEnvironment::globalInstance()->registerAgent( this->getSharedPointer() );
    } else {
        GWSExecutionEnvironment::globalInstance()->unregisterAgent( this->getSharedPointer() );
    }

    // MUST BE MADE AT THIS LAST PART. Add to environments
    GWSEnvironmentsGroup::globalInstance()->registerAgent( this->getSharedPointer() );
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
        foreach (QSharedPointer<GWSObject> s , this->skills->getByClass( GWSSkill::staticMetaObject.className() ) ){
            skills.append( s->serializeMini() );
        }
    }
    //json.insert( "@skills" , skills );

    // BEHAVIOUR
    QJsonArray behaviours;
    if( this->behaviours ){
        foreach (QSharedPointer<GWSObject> s , this->behaviours->getByClass( GWSBehaviour::staticMetaObject.className() ) ){
            behaviours.append( s->serializeMini() );
        }
    }
    //json.insert( "@behaviours" , behaviours );

    // INTERNAL TIME
    json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getSharedPointer() ) );

    // GEOMETRY
    const QSharedPointer<GWSGeometry> geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( this->getSharedPointer() );
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

QSharedPointer<GWSAgent> GWSAgent::getSharedPointer() const{
    QSharedPointer<GWSObject> obj = GWSObject::getSharedPointer();
    return obj.dynamicCast<GWSAgent>();
}

bool GWSAgent::hasSkill( QString class_name ) const{
    return this->skills && this->skills->contains( class_name );
}

QSharedPointer<GWSSkill> GWSAgent::getSkill( QString class_name ) const{
    if( !this->skills ){ return 0; }
    const QList< QSharedPointer<GWSObject> > objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( class_name );
        return 0;
    }
    return objs.at(0).dynamicCast<GWSSkill>();
}

/*template <class T> QSharedPointer<T> GWSAgent::getSkill( QString class_name ) const{
    if( !this->skills ){ return 0; }
    const QList< QSharedPointer<GWSObject> > objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( class_name );
        return 0;
    }
    return objs.at(0).dynamicCast<T>();
}*/

QList< QSharedPointer<GWSSkill> > GWSAgent::getSkills( QString class_name ) const{
    QList< QSharedPointer<GWSSkill> > s;
    if( !this->skills ){ return s; }
    foreach( QSharedPointer<GWSObject> obj , this->skills->getByClass( class_name )){
        s.append( obj.dynamicCast<GWSSkill>() );
    }
    return s;
}

/*template <class T> QList< QSharedPointer<T> > GWSAgent::getSkills( QString class_name ) const{
    QList< QSharedPointer<GWSSkill> > s;
    if( !this->skills ){ return s; }
    foreach( QSharedPointer<GWSObject> obj , this->skills->getByClass( class_name )){
        s.append( obj.dynamicCast<T>() );
    }
    return s;
}*/

QList< QSharedPointer<GWSBehaviour> > GWSAgent::getStartBehaviour() const{
    return this->start_behaviours;
}

QSharedPointer<GWSBehaviour> GWSAgent::getBehaviour( QString id ) const{
    return this->behaviours->getByClassAndId( GWSBehaviour::staticMetaObject.className() , id ).dynamicCast<GWSBehaviour>();
}

QList< QSharedPointer<GWSBehaviour> > GWSAgent::getBehaviours(QString class_name) const{
    QList< QSharedPointer<GWSBehaviour> > behaviours;
    foreach( QSharedPointer<GWSObject> o , this->behaviours->getByClass( class_name ) ){
        behaviours.append( o.dynamicCast<GWSBehaviour>() );
    }
    return behaviours;
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

void GWSAgent::addSkill( QSharedPointer<GWSSkill> skill ){
    if( !this->skills ){
        this->skills = new GWSObjectStorage();
    }
    this->skills->add( skill );
}

void GWSAgent::removeSkill(QSharedPointer<GWSSkill> skill){
    this->skills->remove( skill );
}

void GWSAgent::addBehaviour( QSharedPointer<GWSBehaviour> behaviour){
    if( !this->behaviours ){
        this->behaviours = new GWSObjectStorage();
    }
    this->behaviours->add( behaviour );
}

void GWSAgent::addStartBehaviour( QSharedPointer<GWSBehaviour> behaviour){
    this->start_behaviours.append( behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the agents thread.
 **/
void GWSAgent::tick(){

    this->incrementBusy();
    this->behave();
    this->decrementBusy();

    emit this->agentBehavedSignal();
}

void GWSAgent::behave(){

    // No start behaviour
    if( this->start_behaviours.isEmpty() && this->getProperty( GWSTimeEnvironment::WAIT_FOR_ME_PROP ).toBool() ){
        qWarning() << QString("Agent %1 %2 has no start behaviour. If running, it will probablly block execution time wating for it.").arg( this->metaObject()->className() ).arg( this->getId() );
    }
    if( this->start_behaviours.isEmpty() ){
        return;
    }

    // First behaviour
    QList< QSharedPointer<GWSBehaviour> > checked_behaviours; // TODO check infinite loops
    QList< QSharedPointer<GWSBehaviour> > next_execute_behaviours;

    QList< QSharedPointer<GWSBehaviour> > iterators = this->start_behaviours;

    while( !iterators.isEmpty() ){

        QList< QSharedPointer<GWSBehaviour> > next_loop_iterators;

        // Behaviours
        foreach (QSharedPointer<GWSBehaviour> b, iterators) {

            if( b->finished() ){

                next_loop_iterators.append( b->getNext() );

            } else {

                // SubBehaviours (although Behaviour not finished, maybe some of its subbehaviours has)
                foreach( QSharedPointer<GWSBehaviour> sb, b->getSubs()) {
                    if( sb->finished() ){
                        next_loop_iterators.append( sb->getNext() );
                    }
                }
                next_execute_behaviours.append( b );
            }
        }
        iterators = next_loop_iterators;
    }

    if( !next_execute_behaviours.isEmpty() ){

        qint64 all_start_same_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getSharedPointer() );

        foreach( QSharedPointer<GWSBehaviour> b, next_execute_behaviours) {

            this->timer->singleShot( 10 + (qrand() % 100) , [this , b , all_start_same_time ](){
                b->tick( all_start_same_time );
            });
        }
    } else {
        GWSTimeEnvironment::globalInstance()->incrementAgentInternalTime( this->getSharedPointer() , 1 );
    }
}
