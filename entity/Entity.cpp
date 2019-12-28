#include "Entity.h"

#include <QMetaProperty>
#include <QSqlField>
#include <QJsonValue>
#include <QJsonDocument>
#include <QThread>

#include "../../app/App.h"
#include "../../object/ObjectFactory.h"
#include "../../skill/Skill.h"
#include "../../behaviour/Behaviour.h"
#include "../../util/parallelism/ParallelismController.h"

#include "../../environment/EnvironmentsGroup.h"
#include "../../environment/entity_environment/EntityEnvironment.h"
#include "../../environment/communication_environment/CommunicationEnvironment.h"
#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString geoworldsim::Entity::STYLE_COLOR_PROP = "color";
QString geoworldsim::Entity::STYLE_OPACITY_PROP = "opacity";
QString geoworldsim::Entity::STYLE_BORDER_COLOR_PROP = "border_color";
QString geoworldsim::Entity::STYLE_BORDER_WEIGHT_PROP = "border_weight";
QString geoworldsim::Entity::STYLE_DASH_ARRAY_PROP = "dash_array";
QString geoworldsim::Entity::STYLE_ICON_URL_PROP = "icon_url";
QString geoworldsim::Entity::STYLE_ZOOM_LEVEL_PROP = "zoom_level";

geoworldsim::Entity::Entity() : Object() , busy_counter(0) {
    this->moveToThread( parallel::ParallelismController::globalInstance()->getThread() );
}

geoworldsim::Entity::~Entity() {
    // WARNING!: call deleteLater() using a timer : QTimer::singleShot( 1000 , entity , &geoworldsim::GWSEntity::deleteLater );

    foreach(geoworldsim::environment::Environment* env , this->environments_registerd_in ) {
        env->unregisterEntity( this->getSharedPointer() );
    }

    qDebug() << QString("%1 deleted").arg( this->getUID() );
    if( this->skills ){ this->skills->deleteAllObjects(); this->skills->deleteLater(); }
    if( this->behaviours ){ this->behaviours->deleteAllObjects(); this->behaviours->deleteLater(); }
}

/**********************************************************************
 IMPORTERS
**********************************************************************/

void geoworldsim::Entity::deserialize( const QJsonObject &json , QSharedPointer<Object> parent ){

    QTimer::singleShot( 10 , this , [this , json , parent]{

        Object::deserialize( json , parent );

        // Check if other entity with this ID exists
        QSharedPointer< Entity > existing = environment::EntityEnvironment::globalInstance()->getByClassAndUID( this->metaObject()->className() , this->getUID() );
        if( existing ){
            emit environment::CommunicationEnvironment::globalInstance()->sendMessageSignal(
                        QJsonObject({ { "message" , QString("Duplicated UID in entities %1 %2").arg( existing->getInheritanceFamily().last().toString() ).arg( existing->getUID() ) } }) , geoworldsim::App::globalInstance()->getAppId() + "-LOG" );
            qWarning() << QString("An entity with same UID (class : %1 and UID : %2) already exists. Skipping register.").arg( existing->getInheritanceFamily().last().toString() ).arg( existing->getUID() );
            return;
        }

        // SKILLS
        if( json.keys().contains( "@skills" ) ){
            if( this->skills ){
                this->skills->deleteAllObjects();
                this->skills->deleteLater();
            }
            QJsonArray jskills = json.value("@skills").toArray();
            foreach( QJsonValue js , jskills ){
                QSharedPointer< geoworldsim::skill::Skill > skill = ObjectFactory::globalInstance()->fromJSON( js.toObject() , this->getSharedPointer() ).dynamicCast<geoworldsim::skill::Skill>();
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
                QSharedPointer<geoworldsim::behaviour::Behaviour> behaviour = ObjectFactory::globalInstance()->fromJSON( js.toObject() , this->getSharedPointer() ).dynamicCast<geoworldsim::behaviour::Behaviour>();
                if( behaviour.isNull() ){ continue; }
                this->addBehaviour( behaviour );
            }
        }

        // ADD to MAIN environments !WARNING, DO NOT RE-REGISTER THEM AGAIN
        geoworldsim::environment::EnvironmentsGroup::globalInstance()->registerEntity( this->getSharedPointer() );
    });

}

/**********************************************************************
 EXPORTERS
**********************************************************************/

/**
 * @brief geoworldsim::GWSEntity::serialize Called when asked for more info about this agent.
 * Contains all the additional information of the agent.
 * Can be overwritten if want to add extra variables to child classes
 * @return
 */
QJsonObject geoworldsim::Entity::serialize() const{

    QJsonObject json = Object::serialize();

    //Skills  // DO NOT PROPAGATE THEM TO OTHER ENVIRONMENTS OR UI
    /*QJsonArray skills;
    if( this->skills ){
        foreach (QSharedPointer<GWSObject> s , this->skills->getByClass( geoworldsim::skill::Skill::staticMetaObject.className() ) ){
            skills.append( s->serializeMini() );
        }
    }
    json.insert( "@skills" , skills );
    */

    // BEHAVIOUR  // DO NOT PROPAGATE THEM TO OTHER ENVIRONMENTS OR UI
    /*QJsonArray behaviours;
    if( this->behaviours ){
        foreach( QSharedPointer<GWSObject> s , this->behaviours->getByClass( geoworldsim::behaviour::Behaviour::staticMetaObject.className() ) ){
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

QList<geoworldsim::environment::Environment*> geoworldsim::Entity::getEnvironments() const{
    return this->environments_registerd_in;
}

bool geoworldsim::Entity::isBusy() const{
    return busy_counter > 0;
}

bool geoworldsim::Entity::fulfillsFilter(QJsonObject filter , bool nulls_allowed) const{
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


QSharedPointer<geoworldsim::Entity> geoworldsim::Entity::getSharedPointer() const{
    QSharedPointer<Object> obj = Object::getSharedPointer();
    return obj.dynamicCast<geoworldsim::Entity>();
}

bool geoworldsim::Entity::hasSkill( QString class_name ) const{
    return this->skills && this->skills->contains( class_name );
}

QSharedPointer<geoworldsim::skill::Skill> geoworldsim::Entity::getSkill( QString class_name , bool silent ) const{
    if( !this->skills ){ return Q_NULLPTR; }
    const QList< QSharedPointer<geoworldsim::skill::Skill> > skills = this->skills->getByClass<geoworldsim::skill::Skill>( class_name );
    if( skills.isEmpty() ){
        if( !silent ){ qDebug() << QString("%1:%2 has no skill %3").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( class_name ); }
        return Q_NULLPTR;
    }
    return skills.at(0);
}

/*template <class T> QSharedPointer<T> geoworldsim::GWSEntity::getSkill( QString class_name ) const{
    if( !this->skills ){ return 0; }
    const QList< QSharedPointer<GWSObject> > objs = this->skills->getByClass( class_name );
    if( objs.isEmpty() ){
        qDebug() << QString("%1:%2 has no skills %3").arg( this->metaObject()->className() ).arg( this->getId() ).arg( class_name );
        return 0;
    }
    return objs.at(0).dynamicCast<T>();
}*/

QList< QSharedPointer<geoworldsim::skill::Skill> > geoworldsim::Entity::getSkills( QString class_name ) const{
    if( !this->skills ){ return QList< QSharedPointer<geoworldsim::skill::Skill> >(); }
    return this->skills->getByClass<geoworldsim::skill::Skill>( class_name );
}

/*template <class T> QList< QSharedPointer<T> > geoworldsim::GWSEntity::getSkills( QString class_name ) const{
    QList< QSharedPointer<geoworldsim::skill::Skill> > s;
    if( !this->skills ){ return s; }
    foreach( QSharedPointer<GWSObject> obj , this->skills->getByClass( class_name )){
        s.append( obj.dynamicCast<T>() );
    }
    return s;
}*/

QList< QSharedPointer<geoworldsim::behaviour::Behaviour> > geoworldsim::Entity::getCurrentlyExecutingBehaviours() const{
    return this->to_be_executed_behaviours;
}

QStringList geoworldsim::Entity::getCurrentlyExecutingBehaviourUIDS() const{
    QStringList uids;
    foreach( QSharedPointer< behaviour::Behaviour > b , this->to_be_executed_behaviours ) {
        uids.append( b->getUID() );
    }
    return uids;
}

QSharedPointer< geoworldsim::behaviour::Behaviour > geoworldsim::Entity::getBehaviour( QString id ) const {
    return this->behaviours->getByClassAndUID( behaviour::Behaviour::staticMetaObject.className() , id ).dynamicCast< behaviour::Behaviour >();
}

QList< QSharedPointer< geoworldsim::behaviour::Behaviour > > geoworldsim::Entity::getBehaviours(QString class_name) const{
    if( !this->behaviours ){ return QList< QSharedPointer< behaviour::Behaviour > >(); }
    return this->behaviours->getByClass<  behaviour::Behaviour >( class_name );
}

/**********************************************************************
 SETTERS
**********************************************************************/

void geoworldsim::Entity::incrementBusy(){
    this->busy_counter++;
}

void geoworldsim::Entity::decrementBusy(){
    this->busy_counter = qMax( this->busy_counter-1 , 0 );
}

void geoworldsim::Entity::addSkill( QSharedPointer< geoworldsim::skill::Skill > skill ){
    if( !this->skills ){
        this->skills = new storage::ObjectStorage();
        this->skills->setObjectName( QString("%1 Skills").arg( this->getUID() ) );
    }
    skill->skilled_entity = this->getSharedPointer();
    emit this->skills->addObjectSignal( skill );
}

void geoworldsim::Entity::removeSkill(QSharedPointer<geoworldsim::skill::Skill> skill){
    emit this->skills->removeObjectSignal( skill );
}

void geoworldsim::Entity::addBehaviour( QSharedPointer<geoworldsim::behaviour::Behaviour> behaviour){
    if( !this->behaviours ){
        this->behaviours = new storage::ObjectStorage();
        this->behaviours->setObjectName( QString("%1 Behaviours").arg( this->getUID() ) );
    }
    behaviour->behaving_entity = this->getSharedPointer();
    emit this->behaviours->addObjectSignal( behaviour );
}

void geoworldsim::Entity::addCurrentlyExecutingBehaviour( QSharedPointer<geoworldsim::behaviour::Behaviour> behaviour){
    this->to_be_executed_behaviours.append( behaviour );
}

/**********************************************************************
 SLOTS
**********************************************************************/

void geoworldsim::Entity::run(){
    this->setProperty( geoworldsim::environment::ExecutionEnvironment::ENTITY_BIRTH_PROP , -1 ); // Birth date -1 will be always started
    geoworldsim::environment::ExecutionEnvironment::globalInstance()->registerEntity( this->getSharedPointer() );
}

void geoworldsim::Entity::stop(){
    geoworldsim::environment::ExecutionEnvironment::globalInstance()->unregisterEntity( this->getSharedPointer() );
}

/**
 * This method is a wrapper slot to be invoked by the Environment for behave() to be executed in the entity's thread.
 **/
void geoworldsim::Entity::tick(){

    // Increment has been made in Execution environment
    this->behave();
    this->decrementBusy(); // Decrement is made here

    emit this->entityBehavedSignal();
}

void geoworldsim::Entity::behave(){

    // No start behaviour
    if( this->to_be_executed_behaviours.isEmpty() && this->getProperty( geoworldsim::environment::ExecutionEnvironment::WAIT_FOR_ME_PROP ).toBool() ){
        qWarning() << QString("Entity %1 %2 has no start behaviour and should be waited for it. If running, it will probablly block execution time wating for it.").arg( this->metaObject()->className() ).arg( this->getUID() );
    }

    qint64 behaving_time = this->getProperty( geoworldsim::environment::TimeEnvironment::INTERNAL_TIME_PROP ).toDouble();

    if( this->to_be_executed_behaviours.isEmpty() ){
        qDebug() << QString("Entity %1 %2 has no behaviours to behave.").arg( this->metaObject()->className() ).arg( this->getUID() );
        return;
    }

    double max_behaviour_time_to_increment = 0;
    QJsonArray next_execute_behaviour_ids;

    foreach ( QSharedPointer<geoworldsim::behaviour::Behaviour> behaviour , this->to_be_executed_behaviours ) {

        QPair< double , QJsonArray > duration_and_nexts_ids = behaviour->tick( behaving_time );
        foreach (QJsonValue id , duration_and_nexts_ids.second ) {
            next_execute_behaviour_ids.append( id );
        }

        max_behaviour_time_to_increment = qMax( max_behaviour_time_to_increment , duration_and_nexts_ids.first );
    }

    QList< QSharedPointer<geoworldsim::behaviour::Behaviour> > next_execute_behaviours;
    foreach( QJsonValue v , next_execute_behaviour_ids ) {

        QString id = v.toString();
        if( id.isEmpty() ){ continue; }

        QSharedPointer<geoworldsim::behaviour::Behaviour> behaviour = this->getBehaviour( id );
        if( behaviour.isNull() ){
            qCritical() << QString("Entity %1 %2 requested behaviour %3 but does not exist.").arg( this->metaObject()->className() ).arg( this->getUID() ).arg( id );
            App::globalInstance()->exit( -1 );
        } else {
            next_execute_behaviours.append( behaviour );
        }
    }

    // Store to be executed in next tick
    this->to_be_executed_behaviours = next_execute_behaviours;
    this->setProperty( geoworldsim::environment::TimeEnvironment::INTERNAL_TIME_PROP , behaving_time + (max_behaviour_time_to_increment * 1000) );

}

