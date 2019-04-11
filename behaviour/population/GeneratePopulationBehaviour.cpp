#include "GeneratePopulationBehaviour.h"
#include <QFile>
#include <QTextStream>

#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"
#include "../../environment/agent_environment/AgentEnvironment.h"
#include "../../environment/physical_environment/PhysicalEnvironment.h"
#include "../../object/ObjectFactory.h"

#include "../../util/units/Units.h"

#include "../../util/geometry/GeometryTransformators.h"

#include "../../usecase/PopulationGenerator/PopulationGeneratorAgent.h"

QString GeneratePopulationBehaviour::SIMULATION_LENGTH_YEARS = "years_to_simulate";
QString GeneratePopulationBehaviour::LOOKING_FOR = "looking_for";
QString GeneratePopulationBehaviour::MARRY_AGE = "set_marriage_age";
QString GeneratePopulationBehaviour::MARRY_AGE_MARGIN = "set_marriage_age_margin";
QString GeneratePopulationBehaviour::MARRIAGE_RATE = "marriage_rate";
QString GeneratePopulationBehaviour::COUPLE_ID = "couple_id";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY = "set_life_expectancy_years";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY_MARGIN = "set_life_expectancy_margin";
QString GeneratePopulationBehaviour::ILLNESS_RATE = "illness_rate";
QString GeneratePopulationBehaviour::NEXT_CHILD_GAP = "set_next_child_gap_years";
QString GeneratePopulationBehaviour::BIRTH_RATE = "birth_rate";
QString GeneratePopulationBehaviour::TOTAL_FERTILITY_RATE = "set_fertility_rate";
QString GeneratePopulationBehaviour::MAX_FERTILITY_AGE = "set_max_fertility_age";
QString GeneratePopulationBehaviour::CHILDREN_IDS = "children";
QString GeneratePopulationBehaviour::PARENT1 = "parent1";
QString GeneratePopulationBehaviour::PARENT2 = "parent2";
QString GeneratePopulationBehaviour::NEXT_IF_DIED = "next_if_died";
QString GeneratePopulationBehaviour::NEXT_IF_BIRTH = "next_if_birth";
QString GeneratePopulationBehaviour::NEXT_IF_MIGRATE = "next_if_migrate";
QString GeneratePopulationBehaviour::NEXT_IF_ELSE = "next_if_else";




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

    // Color gradient
    int gradient = qMin( 255.0 , (agent_age / 80.0) * 255.0);
    agent->setProperty( "color" , QString( "rgb(%1,%2,128)" ).arg( gradient ).arg( 255 - gradient ) );

    // Check if the person lives or dies:
    bool died = false;
    died = this->checkDeath( agent_age );
    if( died ){
        return this->getProperty( NEXT_IF_DIED ).toArray();
    }

    bool migrate = false;
    migrate = this->checkMigration( agent_age );
    if(  migrate ){
        return this->getProperty( NEXT_IF_MIGRATE ).toArray();
    }

    bool married = false;
    QString couple_id = agent->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();

    if( couple_id.isNull() ){
        married = checkMarriage( agent_age );
    }

    if( married && checkBirth( agent_age ) ){
        return this->getProperty( NEXT_IF_BIRTH ).toArray();
    }

    return this->getProperty( NEXT_IF_ELSE ).toArray();
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
   // int life_expectancy = agent->getProperty( this->getProperty( LIFE_EXPECTANCY ).toString() ).toInt();
    int life_expectancy = this->getProperty( LIFE_EXPECTANCY ).toInt();

    int testAge = 0;
    bool died = false;

    /*int min = life_expectancy - agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();
    int max = life_expectancy + agent->getProperty( this->getProperty( LIFE_EXPECTANCY_MARGIN ).toString() ).toInt();*/
    int min = life_expectancy - this->getProperty( LIFE_EXPECTANCY_MARGIN ).toInt();
    int max = life_expectancy + this->getProperty( LIFE_EXPECTANCY_MARGIN ).toInt();

    testAge = (qrand() % ( max-min + 1) ) + min;

    QString couple_id = agent->getProperty( COUPLE_ID ).toString();
    QSharedPointer<GWSAgent> couple;

    // Introduce additional randomness for death. Age is not the only cause of death.
     double illness = ( qrand() % ( 100000 )) / 100000.0;
     double illness_rate = this->getProperty( ILLNESS_RATE ).toDouble();

    if ( age >= testAge ||  illness <= illness_rate ) {
        if ( !couple_id.isNull() ){
            couple = GWSAgentEnvironment::globalInstance()->getByUID( couple_id );
            couple->setProperty( COUPLE_ID, QJsonValue() );
        }
        //GWSExecutionEnvironment::globalInstance()->unregisterAgent( agent );
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
    //double marriage_rate = 3.3;
    double marriage_rate = this->getProperty( MARRIAGE_RATE ).toDouble();

    double marriage_ratio = marriage_rate * population / 1000.0 / 100.0 ; // According to data from 2016 EUSTAT

    QSharedPointer<GWSAgent> me = this->getAgent();
   // int my_marrying_age = me->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
   // int my_marrying_margin = me->getProperty( this->getProperty( MARRY_AGE_MARGIN ).toString()  ).toInt();

    int my_marrying_age = this->getProperty( MARRY_AGE ).toInt();
    int my_marrying_margin = this->getProperty( MARRY_AGE_MARGIN ).toInt();


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
    testAge = (qrand() % ( max-min + 1 ) ) + min;

    if ( my_age <= testAge ) {  /* Suitable single candidate, now search a partner */

         GWSGeometry my_geom = GWSPhysicalEnvironment::globalInstance()->getGeometry( me );

         const GWSGeometry buffered_geom = GWSGeometryTransformators::transformBuffer( my_geom , 0.002 );

         // Get total GWSAgents
         QStringList closestCandidates = GWSPhysicalEnvironment::globalInstance()->getAgentsIntersecting( buffered_geom , looking_for );

         // Loop over agents to find suitable companion to breed:
         foreach ( QString candidate_id , closestCandidates ){

                // Check we are not siblings:
                QString myParent1 = me->getProperty( PARENT1 ).toString();
                QString myParent2 = me->getProperty( PARENT2 ).toString();

                QSharedPointer<GWSAgent> candidate = GWSAgentEnvironment::globalInstance()->getByUID( candidate_id );
                QString candidateParent1 = candidate->getProperty( PARENT1 ).toString();
                QString candidateParent2 = candidate->getProperty( PARENT2 ).toString();

                if ( !myParent1.isNull() && !myParent2.isNull() && !candidateParent1.isNull() && !candidateParent2.isNull() ){
                    if ( ( ( myParent1 == candidateParent1 ) && ( myParent2 == candidateParent2 ) ) || ( ( myParent1 == candidateParent2 ) && ( myParent2 == candidateParent1 ) ) ) {
                        continue;
                    }
                }

                // Chek we are suitable for breeding:
                QString candidates_couple_id = candidate->getProperty( this->getProperty( COUPLE_ID ).toString()).toString();
                QString other_looking_for = candidate->getProperty( LOOKING_FOR ).toString();
                bool i_am_candidate = me->getInheritanceFamily().contains( other_looking_for );

                // Check candidate age:
                int candidate_age = this->getAge( candidate );
                int candidate_marriage_age = candidate->getProperty( this->getProperty( MARRY_AGE ).toString() ).toInt();
                int candidate_marriage_age_margin = candidate->getProperty( this->getProperty( MARRY_AGE_MARGIN ).toString() ).toInt();

                int candidatetestAge = 0;
                int min = candidate_marriage_age;
                int max = candidate_marriage_age + candidate_marriage_age_margin;

                candidatetestAge = (qrand() % ( max - min + 1 )  ) + min;

                double value = (qrand() % ( 100000 ) ) / 100000.;

                if ( value <= marriage_ratio  && candidate_age >= candidate_marriage_age  && candidates_couple_id.isNull() && i_am_candidate  ){
                    me->setProperty( this->getProperty( COUPLE_ID ).toString(), candidate->getUID() );
                    candidate->setProperty( this->getProperty( COUPLE_ID ).toString() , me->getUID() );
                    married = true;
                    qDebug() << "Married";
                    break;
                }
                else {
                    continue;
                }
         }
         GWSGeometryTransformators::transformBuffer( buffered_geom , -0.002 );
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

    bool birth = false;

    // Get agent couple:
    QString couple_id = agent->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();
    QSharedPointer<GWSAgent> couple = GWSAgentEnvironment::globalInstance()->getByUID( couple_id );
    int couple_age = this->getAge( couple );

    // If existing children, extract when was last birth:
    int last_birth = 0;
    QList< QSharedPointer<GWSAgent> > children ;

    if ( !agent->getProperty( this->getProperty( CHILDREN_IDS ).toString() ).toArray().isEmpty() ){
        children = GWSAgentEnvironment::globalInstance()->getByUIDS( agent->getProperty( CHILDREN_IDS ).toArray() );

        foreach( QSharedPointer<GWSAgent> child , children ){
            int age = this->getAge( child );
            last_birth = (last_birth == 0 ? age : qMin( age , last_birth ));
        }
    }

    // Get environment parameters:
    int child_gap = this->getProperty( NEXT_CHILD_GAP ).toInt();
    int fertility_rate = this->getProperty( TOTAL_FERTILITY_RATE ).toInt() ;
    int max_fertility_age = this->getProperty( MAX_FERTILITY_AGE ).toInt();

    // If you or your couple are older than the fertility age, cannot reproduce.
    if ( age > max_fertility_age || couple_age > max_fertility_age ){
            // Too old to reproduce
            birth = false;
        }

    // Otherwise, select random threshold age:
    int testAge = 0;
    int min =  this->getProperty( MARRY_AGE ).toInt();

    int max = max_fertility_age;
    testAge = ( qrand() % ( max-min  + 1 )) + min;


    // Increase birth randomness:
    double population = GWSAgentEnvironment::globalInstance()->getAmount();
    //double birth_rate = 7.44; // Per 1000 inhabitants
    double birth_rate = this->getProperty( BIRTH_RATE ).toDouble();
    double birth_ratio = birth_rate * population / 1000. / 100.0; // Bizkaia data from EUSTAT
    double value = ( qrand() % ( 1000000 ) ) / 1000000.;

    if ( value <= birth_ratio && age < testAge && last_birth <= child_gap && children.size() <= fertility_rate  ){

                    birth = true;
                    qDebug() << "Birth";


            }


    return birth;
}

/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/
bool GeneratePopulationBehaviour::checkMigration( int age  )
{
    QSharedPointer<GWSAgent> agent = this->getAgent();

    int testAge = 0;
    int min = 18;
    int max = 45;
    testAge = ( qrand() % ( max - min  + 1 ) ) + min;

    // If you are married or have already had children, you do not move:
    QString couple_id = agent->getProperty( this->getProperty( COUPLE_ID ).toString() ).toString();
    QJsonArray children = agent->getProperty( GeneratePopulationBehaviour::CHILDREN_IDS ).toArray();

    bool migrate = false;
    double value = (qrand() % ( 1000000 )) / 1000000.;

    if ( !couple_id.isNull() || !children.isEmpty() ){
        migrate = false;
    }

    if ( age >= min && age <= testAge && value <= 0.01){
        migrate = true;
        qDebug() << "Migrate";
    }
    else {
        migrate = false;
    }

    return migrate;
}

/*************************************************************************************************************************************
  @name         : GeneratePopulationBehaviour::getAge
  @description  : get age from executing agent
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::getAge( QSharedPointer<GWSAgent> agent ){

    double birth_date = agent->getProperty( GWSExecutionEnvironment::AGENT_BIRTH_PROP ).toDouble();
    int birth_year = QDateTime::fromMSecsSinceEpoch( birth_date ).date().year();

    qint64 current_time = GWSTimeEnvironment::globalInstance()->getCurrentDateTime(); //milliseconds
    int current_year = QDateTime::fromMSecsSinceEpoch( current_time ).date().year();

    return current_year - birth_year;
}
