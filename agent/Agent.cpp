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
#include "../../environment/network_environment/NetworkEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GWSAgent::ALIVE_PROP = "alive";
QString GWSAgent::STYLE_COLOR_PROP = "color";
QString GWSAgent::STYLE_OPACITY_PROP = "opacity";
QString GWSAgent::STYLE_BORDER_COLOR_PROP = "border_color";
QString GWSAgent::STYLE_BORDER_WEIGHT_PROP = "border_weight";
QString GWSAgent::STYLE_DASH_ARRAY_PROP = "dash_array";
QString GWSAgent::STYLE_ICON_URL_PROP = "icon_url";
QString GWSAgent::STYLE_ZOOM_LEVEL_PROP = "zoom_level";

GWSAgent::GWSAgent() : GWSObject() , busy_counter(0) {
    this->setProperty( ALIVE_PROP , true );
}

GWSAgent::~GWSAgent() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , agent , &Agent::deleteLater );

    foreach(GWSEnvironment* env , this->environments_registerd_in ) {
        env->unregisterAgent( this->getSharedPointer() );
    }

    qDebug() << QString("%1 deleted").arg( this->getUID() );
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
        for( int i = jsbehaviours.size()-1 ; i >= 0 ; i-- ){ // Iterate backwards to have @nexts already created
            QJsonValue js = jsbehaviours.at( i );
            QSharedPointer<GWSBehaviour> behaviour = GWSObjectFactory::globalInstance()->fromJSON( js.toObject() , this->getSharedPointer() ).dynamicCast<GWSBehaviour>();
            if( behaviour.isNull() ){ continue; }
            this->addBehaviour( behaviour );
        }
    }

    // ADD to MAIN environments !WARNING, DO NOT RE-REGISTER THEM AGAIN
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

    //Skills  // DO NOT PROPAGATE THEM TO OTHER ENVIRONMENTS OR UI
    /*QJsonArray skills;
    if( this->skills ){
        foreach (QSharedPointer<GWSObject> s , this->skills->getByClass( GWSSkill::staticMetaObject.className() ) ){
            skills.append( s->serializeMini() );
        }
    }
    json.insert( "@skills" , skills );
    */

    // BEHAVIOUR  // DO NOT PROPAGATE THEM TO OTHER ENVIRONMENTS OR UI
    /*QJsonArray behaviours;
    if( this->behaviours ){
        foreach( QSharedPointer<GWSObject> s , this->behaviours->getByClass( GWSBehaviour::staticMetaObject.className() ) ){
            behaviours.append( s->serializeMini() );
        }
    }
    json.insert( "@behaviours" , behaviours );
    */

    // INTERNAL TIME
    {
        qint64 time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getSharedPointer() );
        if( time < 0 ){
            json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , GWSTimeEnvironment::globalInstance()->getCurrentDateTime() );
        } else {
            json.insert( GWSTimeEnvironment::INTERNAL_TIME_PROP , time );
        }
    }

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
      if( !this->skills ){ return Q_NULLPTR; }
    const QList< QSharedPointer<GWSObject> > objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( class_name );
        return Q_NULLPTR;
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

QList< QSharedPointer<GWSBehaviour> > GWSAgent::getCurrentlyExecutingBehaviours() const{
    return this->to_be_executed_behaviours;
}

QSharedPointer<GWSBehaviour> GWSAgent::getBehaviour( QString id ) const {
    return this->behaviours->getByClassAndUID( GWSBehaviour::staticMetaObject.className() , id ).dynamicCast<GWSBehaviour>();
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
    skill->skilled_agent = this->getSharedPointer();
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

void GWSAgent::addCurrentlyExecutingBehaviour( QSharedPointer<GWSBehaviour> behaviour){
    this->to_be_executed_behaviours.append( behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

void GWSAgent::run(){
    this->setProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP , -1 ); // Birth date -1 will be always started
    GWSExecutionEnvironment::globalInstance()->registerAgent( this->getSharedPointer() );
}

void GWSAgent::stop(){
    GWSExecutionEnvironment::globalInstance()->unregisterAgent( this->getSharedPointer() );
}

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the agents thread.
 **/
void GWSAgent::tick(){

    // Increment has been made in Execution environment
    this->behave();
    this->decrementBusy(); // Decrement is made here

    emit this->agentBehavedSignal();
}

void GWSAgent::behave(){

    // No start behaviour
    if( this->to_be_executed_behaviours.isEmpty() && this->getProperty( GWSTimeEnvironment::WAIT_FOR_ME_PROP ).toBool() ){
        qWarning() << QString("Agent %1 %2 has no start behaviour and should be waited for it. If running, it will probablly block execution time wating for it.").arg( this->metaObject()->className() ).arg( this->getUID() );
    }

    qint64 behaving_time = GWSTimeEnvironment::globalInstance()->getAgentInternalTime( this->getSharedPointer() );

    if( this->to_be_executed_behaviours.isEmpty() ){
        qDebug() << QString("Agent %1 %2 has no behaviours to behave.").arg( this->metaObject()->className() ).arg( this->getUID() );
        return;
    }

    double max_behaviour_time_to_increment = 0;
    QJsonArray next_execute_behaviour_ids;

    foreach ( QSharedPointer<GWSBehaviour> behaviour , this->to_be_executed_behaviours ) {
        //qDebug() << QString("AGENT %1 executing %2").arg( this->getUID() ).arg( behaviour->getUID() );
        QJsonArray ids = behaviour->tick( behaving_time );
        foreach (QJsonValue id , ids ) {
            next_execute_behaviour_ids.append( id );
        }
        max_behaviour_time_to_increment = qMax( max_behaviour_time_to_increment , behaviour->getProperty( GWSBehaviour::BEHAVIOUR_DURATION ).toDouble() );
    }

    QList< QSharedPointer<GWSBehaviour> > next_execute_behaviours;
    foreach (QJsonValue v , next_execute_behaviour_ids) {

        QString id = v.toString();
        if( id.isEmpty() ){ continue; }

        QSharedPointer<GWSBehaviour> behaviour = this->getBehaviour( id );
        if( behaviour.isNull() ){
            qCritical() << QString("Agent %1 %2 requested behaviour %3 but does not exist.").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( id );
            GWSApp::exit( -1 );
        } else {
            next_execute_behaviours.append( behaviour );
        }
    }

    // Store to be executed in next tick
    this->to_be_executed_behaviours = next_execute_behaviours;
    GWSTimeEnvironment::globalInstance()->setAgentInternalTime( this->getSharedPointer() , behaving_time + (max_behaviour_time_to_increment * 1000) );
}
