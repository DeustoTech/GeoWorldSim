#include "GeneratePopulationBehaviour.h"
#include <QFile>
#include <QTextStream>

#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"

QString GeneratePopulationBehaviour::SIMULATION_LENGTH_YEARS = "years_to_simulate";
QString GeneratePopulationBehaviour::LOOKING_FOR = "looking_for";
QString GeneratePopulationBehaviour::MARRY_AGE = "set_marriage_age";
QString GeneratePopulationBehaviour::COUPLE_ID = "store_couple_id_as";
QString GeneratePopulationBehaviour::MARRY_AGE_MARGIN = "set_marriage_age_margin";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY = "set_life_expectancy_years";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY_MARGIN = "set_life_expectancy_margin";
QString GeneratePopulationBehaviour::NEXT_CHILD_GAP = "set_next_child_gap_years";
QString GeneratePopulationBehaviour::TOTAL_FERTILITY_RATE = "set_fertility_rate";
QString GeneratePopulationBehaviour::MAX_FERTILITY_AGE = "set_max_fertility_age";
QString GeneratePopulationBehaviour::NEXT = "next";
QString GeneratePopulationBehaviour::CHILDREN_IDS = "store_children_as";
QString GeneratePopulationBehaviour::PARENT1;
QString GeneratePopulationBehaviour::PARENT2;



GeneratePopulationBehaviour::GeneratePopulationBehaviour() : GWSBehaviour()
{

}


/**********************************************************************
 INITIALIZE
**********************************************************************/

void GeneratePopulationBehaviour::initialize(){

    QSharedPointer<GWSAgent> agent = this->getAgent();
    agent->setProperty( CHILDREN_IDS , QJsonArray() );
}


/**********************************************************************
 SLOTS
**********************************************************************/

QJsonArray GeneratePopulationBehaviour::behave(){

    QSharedPointer<GWSAgent> agent = this->getAgent();

    // Get agent age:
    int agent_age = this->getAge( agent );

    // Check if the person lives or dies:
    bool died = this->checkDeath( agent_age );
    if( died ){
        return this->getProperty( NEXT ).toArray();
    }

    bool married = this->checkMarriage( agent_age );
    if( married ){
        this->checkBirth( agent_age );
    }

    return this->getProperty( NEXT ).toArray();
}



/*************************************************************************************************************************************
  @name         :
  @description  : Checks if an individual has reached his life expectancy. Handle death scenario and counters.
  @param
  @return
*************************************************************************************************************************************/
bool GeneratePopulationBehaviour::checkDeath( int age )
{
    QSharedPointer<GWSAgent> agent = this->getAgent();

    quint64 testAge = 0;
    bool died = false;

    int min = agent->getProperty( this->getProperty( LIFE_EXPECTANCY ).toString() ).toInt() - agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();
    int max = agent->getProperty( this->getProperty( LIFE_EXPECTANCY ).toString() ).toInt() + agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();
    testAge = this->generateRandom( min, max );

    // Check for couple:
    QString couple_id = agent->getProperty( COUPLE_ID ).toString();
    QSharedPointer<GWSAgent> couple;

    if ( !couple_id.isNull() ){
        couple = GWSAgentEnvironment::globalInstance()->getById( couple_id );
    }

    if ( age > testAge )
        {

        if ( !couple.isNull() ){
            couple->setProperty( COUPLE_ID, QJsonValue() );
        }
        GWSExecutionEnvironment::globalInstance()->unregisterAgent( agent );
        died = true;
        }
     else
        {
        died = false;
        }

    return died;
}





/*************************************************************************************************************************************
  @name         :
  @description  : Check if a person is eligible for marriage. Choose a suitable partner. get them married and add to the couple list.
  @param
  @return
*************************************************************************************************************************************/
bool GeneratePopulationBehaviour::checkMarriage( int my_age  ){

    QSharedPointer<GWSAgent> me = this->getAgent();
    int my_marrying_age = me->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
    QString looking_for = me->getProperty( LOOKING_FOR ).toString();
    QString couple_id = me->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();

    bool married = false;

    /* Check if agent already has a couple */
    if ( !couple_id.isNull() ){
        married = true;
    }
    else {

        /* Check if agent has reached his legal marriage age */
        if ( my_age < my_marrying_age){
            married = false;
            return married;
        }

        else { /* Suitable single male candidate, now search a partner for him. */

             // Get total GWSAgents
             QList<QSharedPointer<GWSAgent>> candidates = GWSAgentEnvironment::globalInstance()->getByClass( looking_for );

             // Loop over agents to find suitable companion to breed:
             foreach ( QSharedPointer<GWSAgent> candidate , candidates ){

                    int candidate_age = this->getAge( candidate );
                    int candidate_marriage_age = candidate->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
                    QString candidates_couple_id = candidate->getProperty( this->getProperty( COUPLE_ID ).toString()).toString();
                    QString other_looking_for = candidate->getProperty( LOOKING_FOR ).toString();
                    bool i_am_candidate = me->getInheritanceFamily().contains( other_looking_for );

                    if ( candidate_age >= candidate_marriage_age  && candidates_couple_id.isNull() && i_am_candidate ){
                        me->setProperty( this->getProperty( COUPLE_ID ).toString(), candidate->getId() );
                        candidate->setProperty( this->getProperty( COUPLE_ID ).toString() , me->getId() );
                        married = true;
                        break;
                    }
                    else {
                        continue;
                    }
             }
          }

       }

return married;

}


/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/
bool GeneratePopulationBehaviour::checkBirth( int age  )
{

    QSharedPointer<GWSAgent> agent = this->getAgent();

    QString couple_id = agent->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();
    QSharedPointer<GWSAgent> couple = GWSAgentEnvironment::globalInstance()->getById( couple_id );
    int couple_age = this->getAge( couple );

    int last_birth = 0;

    QList< QSharedPointer<GWSAgent> > children ;
    if ( !agent->getProperty( this->getProperty( CHILDREN_IDS ).toString() ).toArray().isEmpty() ){
        children = GWSAgentEnvironment::globalInstance()->getByIds( agent->getProperty( CHILDREN_IDS ).toArray() );

        foreach( QSharedPointer<GWSAgent> child , children ){
            int age = this->getAge( child );
            last_birth = (last_birth == 0 ? age : qMin( age , last_birth ));
        }

    }

    int child_gap = this->getProperty( NEXT_CHILD_GAP ).toInt();
    int fertility_rate = this->getProperty( TOTAL_FERTILITY_RATE ).toInt() ;
    int max_fertility_age = this->getProperty( MAX_FERTILITY_AGE ).toInt();
    bool birth = false;

    if ( age > max_fertility_age || couple_age > max_fertility_age ){
            // Too old to reproduce
            birth = false;
        }

    else {
            if ( last_birth <= child_gap && children.size() <= fertility_rate ){

                    /* Reproduce */

                    int testSample = 0;
                    int min = 1;
                    int max = 10 ;
                    testSample = this->generateRandom(min, max);

                    QSharedPointer<GWSAgent> new_born;
                    new_born->setProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP, GWSTimeEnvironment::globalInstance()->getCurrentDateTime());

                    GWSExecutionEnvironment::globalInstance()->registerAgent( new_born );
                    birth = true;
                   // new_born->setProperty( GeneratePopulationBehaviour::PARENT1, agent->getId() );
                    new_born->setProperty( GeneratePopulationBehaviour::PARENT2, agent->getProperty( COUPLE_ID ).toString());

                    if ( testSample <= 5 )
                    {
                        /* Baby girl */
                        new_born->setProperty( this->LOOKING_FOR, "MALE" );
                    }
                    else {
                        /* Baby boy*/
                        new_born->setProperty( this->LOOKING_FOR, "FEMALE" );
                    }


                    QJsonArray children_ids;
                    foreach( QSharedPointer<GWSAgent> child , children ){
                        children_ids.append( child->getId() );
                    }
                    children_ids.append( new_born->getId() );
                    agent->setProperty( GeneratePopulationBehaviour::CHILDREN_IDS , children_ids );
                    couple->setProperty( GeneratePopulationBehaviour::CHILDREN_IDS , children_ids );
            }
        }

    return birth;
}



/**********************************************************************
@name           :
@description    :
@param
@return
**********************************************************************/
double GeneratePopulationBehaviour::generateRandom(int min, int max)
{
    unsigned int num = 0;
    unsigned int tmp = 0;
    struct timeval tp;

    gettimeofday(&tp, NULL);
    srand(tp.tv_usec);
    num = rand();

    /* restirict within min and max */
    tmp = (num % max) + min;

    /* Wrap around */
    if (tmp > max)
    {
        num = min + (tmp - max);
    }
    else
    {
        num = tmp;
    }

    return num;
}




int GeneratePopulationBehaviour::getAge( QSharedPointer<GWSAgent> agent ){

    quint64 birth_date = agent->getProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP ).toDouble();
    quint64 birth_year = QDateTime::fromMSecsSinceEpoch( birth_date ).date().year();

    quint64 current_time = GWSTimeEnvironment::globalInstance()->getCurrentDateTime(); //milliseconds
    quint64 current_year = QDateTime::fromMSecsSinceEpoch( current_time ).date().year();

    return current_year - birth_year;
}
