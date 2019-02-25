#include "GeneratePopulationBehaviour.h"
#include <QFile>
#include <QTextStream>

#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../object/ObjectFactory.h"

#include "../../util/units/Units.h"

#include "../../usecase/PopulationGenerator/PopulationGeneratorAgent.h"

QString GeneratePopulationBehaviour::SIMULATION_LENGTH_YEARS = "years_to_simulate";
QString GeneratePopulationBehaviour::LOOKING_FOR = "looking_for";
QString GeneratePopulationBehaviour::MARRY_AGE = "set_marriage_age";
QString GeneratePopulationBehaviour::COUPLE_ID = "couple_id";
QString GeneratePopulationBehaviour::MARRY_AGE_MARGIN = "set_marriage_age_margin";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY = "set_life_expectancy_years";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY_MARGIN = "set_life_expectancy_margin";
QString GeneratePopulationBehaviour::NEXT_CHILD_GAP = "set_next_child_gap_years";
QString GeneratePopulationBehaviour::TOTAL_FERTILITY_RATE = "set_fertility_rate";
QString GeneratePopulationBehaviour::MAX_FERTILITY_AGE = "set_max_fertility_age";
QString GeneratePopulationBehaviour::NEXT = "next";
QString GeneratePopulationBehaviour::CHILDREN_IDS = "children";
QString GeneratePopulationBehaviour::PARENT1 = "parent1";
QString GeneratePopulationBehaviour::PARENT2 = "parent2";



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
    int life_expectancy = agent->getProperty( this->getProperty( LIFE_EXPECTANCY ).toString() ).toInt();

    int testAge = 0;
    bool died = false;

    int min = life_expectancy - agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();
    int max = life_expectancy + agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();
    testAge = QRandomGenerator::global()->bounded( min, max );

    QString couple_id = agent->getProperty( COUPLE_ID ).toString();
    QSharedPointer<GWSAgent> couple;

    // Introduce additional randomness for death. Age is not the only cause of death.
     double illness = QRandomGenerator::global()->generateDouble();
   // qDebug() << illness;


    if ( age >= testAge ||  illness <= 0.01 )
        {

        if ( !couple_id.isNull() ){
            couple = GWSAgentEnvironment::globalInstance()->getById( couple_id );
            couple->setProperty( COUPLE_ID, QJsonValue() );
        }
        GWSExecutionEnvironment::globalInstance()->unregisterAgent( agent );
        died = true;
        qDebug() << age << "You die!";
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

    bool married = false;

    // Increase marriage randomness:
    double population = GWSAgentEnvironment::globalInstance()->getAmount();
    double marriage_ratio = 3.3 * population / 1000.0 / 100.0 ; // According to data from 2016 EUSTAT

    QSharedPointer<GWSAgent> me = this->getAgent();
    int my_marrying_age = me->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
    int my_marrying_margin = me->getProperty( this->getProperty( MARRY_AGE_MARGIN ).toString()  ).toInt();

    QString looking_for = me->getProperty( LOOKING_FOR ).toString();
    QString couple_id = me->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();

    if ( my_age < my_marrying_age ){
        married = false;
        return married;
    }

    /* Check if agent already has a couple */
    if ( !couple_id.isNull() ){
        married = true;
        return married;
    }


    int testAge = 0;
    int min = my_marrying_age;
    int max = my_marrying_age + my_marrying_margin;
    testAge = QRandomGenerator::global()->bounded( min, max );



    if ( my_age >= testAge ) {  /* Suitable single candidate, now search a partner */

         GWSPhysicalEnvironment::globalInstance()->transformBuffer( me , 0.002 );

         GWSCoordinate myCoordinates = GWSPhysicalEnvironment::globalInstance()->getGeometry( me )->getCentroid();

         // Get total GWSAgents
         QList< QSharedPointer<GWSAgent> > closestCandidates = GWSPhysicalEnvironment::globalInstance()->getAgentsIntersecting( GWSPhysicalEnvironment::globalInstance()->getGeometry( me ) , looking_for );

         /*QList<GWSCoordinate> candidateCoors;
         foreach ( QSharedPointer<GWSAgent> candidate , candidates ){
            GWSCoordinate coor = GWSPhysicalEnvironment::globalInstance()->getGeometry( candidate )->getCentroid();
            candidateCoors.append( coor );
         }

         QList<QSharedPointer<GWSAgent>> closestCandidates = GWSPhysicalEnvironment::globalInstance()->getNearestAgents( candidateCoors, looking_for );
        */

         // Loop over agents to find suitable companion to breed:
         foreach ( QSharedPointer<GWSAgent> candidate , closestCandidates ){

                int candidate_age = this->getAge( candidate );
                int candidate_marriage_age = candidate->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
                int candidate_marriage_age_margin = candidate->getProperty( this->getProperty( MARRY_AGE_MARGIN ).toString() ).toInt();

                QString candidates_couple_id = candidate->getProperty( this->getProperty( COUPLE_ID ).toString()).toString();
                QString other_looking_for = candidate->getProperty( LOOKING_FOR ).toString();
                bool i_am_candidate = me->getInheritanceFamily().contains( other_looking_for );

                int candidatetestAge = 0;
                int min = candidate_marriage_age;
                int max = candidate_marriage_age + candidate_marriage_age_margin;

                //candidatetestAge = this->generateRandom(min, max);
                candidatetestAge = QRandomGenerator::global()->bounded( min, max );

                //double random_number = qrand();
                double value = QRandomGenerator::global()->generateDouble();
                //qDebug() << value;

                if ( value <= marriage_ratio  && candidate_age >= candidate_marriage_age  && candidates_couple_id.isNull() && i_am_candidate  ){
                    me->setProperty( this->getProperty( COUPLE_ID ).toString(), candidate->getId() );
                    candidate->setProperty( this->getProperty( COUPLE_ID ).toString() , me->getId() );
                    married = true;
                    qDebug() << "Married";
                    break;
                }
                else {
                    continue;
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

    // Increase birth randomness:
   // double births_per_year = 120;
    double population = GWSAgentEnvironment::globalInstance()->getAmount();
    double birth_ratio = 7.44 * population / 1000. / 100.0; // Bizkaia data from EUSTAT

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

    int testAge = 0;
    int min = agent->getProperty( MARRY_AGE ).toInt();
    int max = max_fertility_age;
   // testAge = this->generateRandom(min, max);
    testAge = QRandomGenerator::global()->bounded( min, max );


    if ( age > max_fertility_age || couple_age > max_fertility_age ){
            // Too old to reproduce
            birth = false;
        }

    double value = QRandomGenerator::global()->generateDouble();
    //qDebug() << value;
    if ( value <= birth_ratio && age < testAge && last_birth <= child_gap && children.size() <= fertility_rate  ){

                    // Pass parent information as basic characteristics:
                    QJsonObject new_born_json = agent->serialize();
                    QDateTime next_year = QDateTime::fromMSecsSinceEpoch( GWSTimeEnvironment::globalInstance()->getAgentInternalTime( agent ) ).addYears( 1 );
                    new_born_json.insert( GWSExecutionEnvironment::AGENT_BIRTH_PROP , next_year.toMSecsSinceEpoch());
                    new_born_json.insert( GWSAgent::GWS_ID_PROP , QJsonValue() );
                    new_born_json.insert( "@behaviours" , QJsonValue() );
                    new_born_json.insert( GeneratePopulationBehaviour::COUPLE_ID , QString() );

                    new_born_json.insert( GeneratePopulationBehaviour::PARENT1, agent->getId() );
                    new_born_json.insert( GeneratePopulationBehaviour::PARENT2, couple_id );
                    new_born_json.insert( GeneratePopulationBehaviour::CHILDREN_IDS , QJsonArray() );

                    // Add behaviours
                    QList< QSharedPointer<GWSBehaviour> > behaviours = agent->getBehaviours( GWSBehaviour::staticMetaObject.className() );
                    if ( !behaviours.isEmpty() ) {
                        QJsonArray arr;
                        foreach ( QSharedPointer<GWSBehaviour> o , behaviours ){
                                arr.append( o->serialize() );
                        }
                        new_born_json.insert( "@behaviours" , arr );
                    }

                    // Register through object factory:
                    QSharedPointer<GWSAgent> new_born = GWSObjectFactory::globalInstance()->fromJSON( new_born_json ).dynamicCast<GWSAgent>();
                    birth = true;
                    qDebug() << "Birth";
                    QJsonArray children_ids;
                    foreach( QSharedPointer<GWSAgent> child , children ){
                        children_ids.append( child->getId() );
                    }
                    children_ids.append( new_born->getId() );
                    agent->setProperty( GeneratePopulationBehaviour::CHILDREN_IDS , children_ids );
                    couple->setProperty( GeneratePopulationBehaviour::CHILDREN_IDS , children_ids );
            }


    return birth;
}



/**********************************************************************
@name           :
@description    :
@param
@return
**********************************************************************/
/*double GeneratePopulationBehaviour::generateRandom(int min, int max)
{
    unsigned int num = 0;
    unsigned int tmp = 0;
    struct timeval tp;

    gettimeofday(&tp, NULL);
    srand(tp.tv_usec);
    num = rand();

    // restirict within min and max
    tmp = (num % max) + min;

    // Wrap around
    if (tmp > max)
    {
        num = min + (tmp - max);
    }
    else
    {
        num = tmp;
    }

    return num;
} */




int GeneratePopulationBehaviour::getAge( QSharedPointer<GWSAgent> agent ){

    quint64 birth_date = agent->getProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP ).toDouble();
    quint64 birth_year = QDateTime::fromMSecsSinceEpoch( birth_date ).date().year();

    quint64 current_time = GWSTimeEnvironment::globalInstance()->getCurrentDateTime(); //milliseconds
    quint64 current_year = QDateTime::fromMSecsSinceEpoch( current_time ).date().year();

    return current_year - birth_year;
}
