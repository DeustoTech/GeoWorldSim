#include "Entity.h"

#include <QMetaProperty>
#include <QSqlField>
#include <QJsonValue>
#include <QJsonDocument>
#include <QThread>

#include "../../app/App.h"
#include "../../util/parallelism/ParallelismController.h"
#include "../../object/ObjectFactory.h"
#include "../../behaviour/Behaviour.h"
#include "../../skill/Skill.h"

#include "../../environment/Environment.h"
#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"


QString GWSEntity::STYLE_COLOR_PROP = "color";
QString GWSEntity::STYLE_OPACITY_PROP = "opacity";
QString GWSEntity::STYLE_BORDER_COLOR_PROP = "border_color";
QString GWSEntity::STYLE_BORDER_WEIGHT_PROP = "border_weight";
QString GWSEntity::STYLE_DASH_ARRAY_PROP = "dash_array";
QString GWSEntity::STYLE_ICON_URL_PROP = "icon_url";
QString GWSEntity::STYLE_ZOOM_LEVEL_PROP = "zoom_level";

GWSEntity::GWSEntity() : GWSObject() , busy_counter(0) {
    this->moveToThread( GWSParallelismController::globalInstance()->getThread() );
}

GWSEntity::~GWSEntity() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , entity , &GWSEntity::deleteLater );

    foreach(GWSEnvironment* env , this->environments_registerd_in ) {
        env->unregisterEntity( this->getSharedPointer() );
    }

    qDebug() << QString("%1 deleted").arg( this->getUID() );
    if( this->skills ){ this->skills->deleteAllObjects(); this->skills->deleteLater(); }
    if( this->behaviours ){ this->behaviours->deleteAllObjects(); this->behaviours->deleteLater(); }
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void GWSEntity::deserialize( const QJsonObject &json , QSharedPointer<GWSObject> parent ){
    GWSObject::deserialize( json , parent );

    QTimer::singleShot( 10 , this , [this , json]{

        // SKILLS
        if( json.keys().contains( "@skills" ) ){
            if( this->skills ){
                this->skills->deleteAllObjects();
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
                this->behaviours->deleteAllObjects();
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
        GWSEnvironmentsGroup::globalInstance()->registerEntity( this->getSharedPointer() );
    });

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/**
 * @brief GWSEntity::serialize Called when asked for more info about this agent.
 * Contains all the additional information of the agent.
 * Can be overwritten if want to add extra variables to child classes
 * @return
 */
QJsonObject GWSEntity::serialize() const{

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

    return json;
}

/**
 * @brief Entity::toImage
 * @return
 */
/*QImage Entity::toImage(const GWSEnvelope image_bounds, unsigned int image_width, unsigned int image_height ){
    return this->geometry->toImage( image_bounds , image_width , image_height , this->getStyle()->border_color , this->getStyle()->border_weight , this->getStyle()->color , this->getStyle()->icon );
}*/


/**********************************************************************
 GETTERS
**********************************************************************/

QList<GWSEnvironment*> GWSEntity::getEnvironments() const{
    return this->environments_registerd_in;
}

bool GWSEntity::isBusy() const{
    return busy_counter > 0;
}

bool GWSEntity::fulfillsFilter(QJsonObject filter , bool nulls_allowed) const{
    bool valid = true;
    foreach( QString key , filter.keys() ){

        QJsonValue v = this->getProperty( key );

        if( v != filter.value( key ) ){
            valid = false;
        }

        if( v.isNull() && nulls_allowed ){
            valid = true;
        }

        if( !valid ){
            break;
        }
    }
    return valid;
}




QSharedPointer<GWSEntity> GWSEntity::getSharedPointer() const{
    QSharedPointer<GWSObject> obj = GWSObject::getSharedPointer();
    return obj.dynamicCast<GWSEntity>();
}

bool GWSEntity::hasSkill( QString class_name ) const{
    return this->skills && this->skills->contains( class_name );
}

QSharedPointer<GWSSkill> GWSEntity::getSkill( QString class_name , bool silent ) const{
    if( !this->skills ){ return Q_NULLPTR; }
    const QList< QSharedPointer<GWSSkill> > skills = this->skills->getByClass<GWSSkill>( class_name );
    if( skills.isEmpty() ){
        if( !silent ){ qDebug() << QString("%1:%2 has no skill %3").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( class_name ); }
        return Q_NULLPTR;
    }
    return skills.at(0);
}

/*template <class T> QSharedPointer<T> GWSEntity::getSkill( QString class_name ) const{
    if( !this->skills ){ return 0; }
    const QList< QSharedPointer<GWSObject> > objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( class_name );
        return 0;
    }
    return objs.at(0).dynamicCast<T>();
}*/

QList< QSharedPointer<GWSSkill> > GWSEntity::getSkills( QString class_name ) const{
    if( !this->skills ){ return QList< QSharedPointer<GWSSkill> >(); }
    return this->skills->getByClass<GWSSkill>( class_name );
}

/*template <class T> QList< QSharedPointer<T> > GWSEntity::getSkills( QString class_name ) const{
    QList< QSharedPointer<GWSSkill> > s;
    if( !this->skills ){ return s; }
    foreach( QSharedPointer<GWSObject> obj , this->skills->getByClass( class_name )){
        s.append( obj.dynamicCast<T>() );
    }
    return s;
}*/

QList< QSharedPointer<GWSBehaviour> > GWSEntity::getCurrentlyExecutingBehaviours() const{
    return this->to_be_executed_behaviours;
}

QStringList GWSEntity::getCurrentlyExecutingBehaviourUIDS() const{
    QStringList uids;
    foreach (QSharedPointer<GWSBehaviour> b , this->to_be_executed_behaviours ) {
        uids.append( b->getUID() );
    }
    return uids;
}

QSharedPointer<GWSBehaviour> GWSEntity::getBehaviour( QString id ) const {
    return this->behaviours->getByClassAndUID( GWSBehaviour::staticMetaObject.className() , id ).dynamicCast<GWSBehaviour>();
}

QList< QSharedPointer<GWSBehaviour> > GWSEntity::getBehaviours(QString class_name) const{
    if( !this->behaviours ){ return QList< QSharedPointer<GWSBehaviour> >(); }
    return this->behaviours->getByClass<GWSBehaviour>( class_name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void GWSEntity::incrementBusy(){
    this->busy_counter++;
}

void GWSEntity::decrementBusy(){
    this->busy_counter = qMax( this->busy_counter-1 , 0 );
}

void GWSEntity::addSkill( QSharedPointer<GWSSkill> skill ){
    if( !this->skills ){
        this->skills = new GWSObjectStorage();
    }
    skill->skilled_entity = this->getSharedPointer();
    emit this->skills->addObjectSignal( skill );
}

void GWSEntity::removeSkill(QSharedPointer<GWSSkill> skill){
    emit this->skills->removeObjectSignal( skill );
}

void GWSEntity::addBehaviour( QSharedPointer<GWSBehaviour> behaviour){
    if( !this->behaviours ){
        this->behaviours = new GWSObjectStorage();
    }
    behaviour->behaving_entity = this->getSharedPointer();
    emit this->behaviours->addObjectSignal( behaviour );
}

void GWSEntity::addCurrentlyExecutingBehaviour( QSharedPointer<GWSBehaviour> behaviour){
    this->to_be_executed_behaviours.append( behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

void GWSEntity::run(){
    this->setProperty( GWSExecutionEnvironment::ENTITY_BIRTH_PROP , -1 ); // Birth date -1 will be always started
    GWSExecutionEnvironment::globalInstance()->registerEntity( this->getSharedPointer() );
}

void GWSEntity::stop(){
    GWSExecutionEnvironment::globalInstance()->unregisterEntity( this->getSharedPointer() );
}

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the entity's thread.
 **/
void GWSEntity::tick(){

    // Increment has been made in Execution environment
    this->behave();
    this->decrementBusy(); // Decrement is made here

    emit this->entityBehavedSignal();
}

void GWSEntity::behave(){

    // No start behaviour
    if( this->to_be_executed_behaviours.isEmpty() && this->getProperty( GWSExecutionEnvironment::WAIT_FOR_ME_PROP ).toBool() ){
        qWarning() << QString("Entity %1 %2 has no start behaviour and should be waited for it. If running, it will probablly block execution time wating for it.").arg( this->metaObject()->className() ).arg( this->getUID() );
    }

    qint64 behaving_time = this->getProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    if( this->to_be_executed_behaviours.isEmpty() ){
        qDebug() << QString("Entity %1 %2 has no behaviours to behave.").arg( this->metaObject()->className() ).arg( this->getUID() );
        return;
    }

    double max_behaviour_time_to_increment = 0;
    QJsonArray next_execute_behaviour_ids;

    foreach ( QSharedPointer<GWSBehaviour> behaviour , this->to_be_executed_behaviours ) {

        QPair< double , QJsonArray > duration_and_nexts_ids = behaviour->tick( behaving_time );
        foreach (QJsonValue id , duration_and_nexts_ids.second ) {
            next_execute_behaviour_ids.append( id );
        }

        max_behaviour_time_to_increment = qMax( max_behaviour_time_to_increment , duration_and_nexts_ids.first );
    }

    QList< QSharedPointer<GWSBehaviour> > next_execute_behaviours;
    foreach( QJsonValue v , next_execute_behaviour_ids ) {

        QString id = v.toString();
        if( id.isEmpty() ){ continue; }

        QSharedPointer<GWSBehaviour> behaviour = this->getBehaviour( id );
        if( behaviour.isNull() ){
            qCritical() << QString("Entity %1 %2 requested behaviour %3 but does not exist.").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( id );
            GWSApp::globalInstance()->exit( -1 );
        } else {
            next_execute_behaviours.append( behaviour );
        }
    }

    // Store to be executed in next tick
    this->to_be_executed_behaviours = next_execute_behaviours;
    this->setProperty( GWSTimeEnvironment::INTERNAL_TIME_PROP , behaving_time + (max_behaviour_time_to_increment * 1000) );
}
