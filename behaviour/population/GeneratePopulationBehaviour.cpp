#include "GeneratePopulationBehaviour.h"
#include <QFile>
#include <QTextStream>

#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include "../../environment/time_environment/TimeEnvironment.h"

QString GeneratePopulationBehaviour::SIMULATION_LENGTH_YEARS = "years_to_simulate";
QString GeneratePopulationBehaviour::INITIAL_MALE_AGE = "set_first_males_age";
QString GeneratePopulationBehaviour::INITIAL_FEMALE_AGE = "set_first_females_age";
QString GeneratePopulationBehaviour::MARRY_AGE = "set_marriage_age";
QString GeneratePopulationBehaviour::MARRY_AGE_MARGIN = "set_marriage_age_margin";
QString GeneratePopulationBehaviour::LIFE_EXPECTANCY = "set_life_expectancy_years";
QString GeneratePopulationBehaviour::NEXT_CHILD_GAP = "set_next_child_gap_years";
QString GeneratePopulationBehaviour::TOTAL_FERTILITY_RATE = "set_fertility_rate";
QString GeneratePopulationBehaviour::MAX_FERTILITY_AGE = "set_max_fertility_age";
QString GeneratePopulationBehaviour::NEXT = "next";

GeneratePopulationBehaviour::GeneratePopulationBehaviour() : GWSBehaviour()
{

}


/**********************************************************************
 SLOTS
**********************************************************************/

QJsonArray GeneratePopulationBehaviour::behave(){

    // Get this agent's details
    quint64 birth_date = this->getProperty( GWSExecutionEnvironment::BIRTH_PROP ).toDouble();
    quint64 current_time = GWSTimeEnvironment::globalInstance()->getCurrentDateTime(); //milliseconds
    quint64 current_year = QDate::currentDate().year();
    quint64 agent_age = current_year - birth_date;

    int marry_age =         this->getProperty( MARRY_AGE ).toInt();
    int marry_age_margin =  this->getProperty( MARRY_AGE_MARGIN ).toInt();
    int life_expectancy =   this->getProperty( LIFE_EXPECTANCY ).toInt();
    int life_expectancy_margin = this->getProperty( LIFE_EXPECTANCY_MARGIN ).toInt();
    int next_child_gap =    this->getProperty( NEXT_CHILD_GAP ).toInt();
    int total_fertility_rate =  this->getProperty( TOTAL_FERTILITY_RATE ).toInt();
    int max_fertility_age =     this->getProperty( MAX_FERTILITY_AGE ).toInt();

    int retval = 0;
    int yearsToAdvance = this->getProperty( SIMULATION_LENGTH_YEARS ).toInt();

    /* Initialize global parameters */
    //retval = this->launch();
    //retval = this->simulate(yearsToAdvance);

    this->checkDeath( agent_age );

    if ( retval != SUCCESS )
    {
       qDebug() << "ERROR: Could not simulate!";
    }


    return this->getProperty( NEXT ).toArray();
}





/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/

int GeneratePopulationBehaviour::launch(){

    /* Parameters */
    globalParam.marry_ageMale = 21;
    globalParam.marry_ageFemale = 18;
    globalParam.marry_age_margin = 15;
    globalParam.life_expectancy = 85;
    globalParam.life_expectancy_margin = 20;
    globalParam.next_child_gap = 3;
    globalParam.total_fertility_rate = 6;
    globalParam.max_male_fertility_age = 60;
    globalParam.max_female_fertility_age = 45;

    /* Statistics */
    globalStats.mIndex = 0;
    globalStats.fIndex = 0;
    globalStats.totalMaleAge = 0;
    globalStats.totalFemaleAge = 0;
    globalStats.totalCouples = 0;
    globalStats.totalMaleDeath = 0;
    globalStats.totalFemaleDeath = 0;
    globalStats.totalYears = 1;
    globalStats.birthsThisYear = 0;
    globalStats.deathsThisYear = 0;
    globalStats.totalChildren = 0;

    /* Adam & Eve */
    person_t m1;
    person_t f1;

    m1.id = 1;

    //m1.age = globalParam.marryAgeMale;
    m1.age = this->getProperty( INITIAL_MALE_AGE ).toInt();
    f1.id = 1;
    //f1.age = globalParam.marryAgeFemale;
    f1.age = this->getProperty( INITIAL_FEMALE_AGE ).toInt();

    this->addPerson(MALE, m1);
    this->addPerson(FEMALE, f1);

    qDebug() << "Example" << globalParam.marry_ageMale;

    return SUCCESS;
}


/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::addPerson( int sex, person_t person  )
{
    int retval = 0;

    if ( sex == MALE )
    {
        mList.push_back(person);
        //globalStats.mIndex++;
        //globalStats.totalMales++;
        //globalStats.totalMaleAge =+ person.age;
    }
    else
    {
        fList.push_back(person);
       // globalStats.fIndex++;
        //globalStats.totalFemales++;
        //globalStats.totalFemaleAge =+ person.age;
    }

    // MAIALEN: write added people to .csv file
    QFile csv( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/PopulationGenerator/census.csv" );
    if( csv.open( QIODevice::ReadWrite ) ) {
           QTextStream stream(&csv);
           stream << person.id <<";" << sex  <<";" << person.age << endl;
       }
    csv.close();

    return SUCCESS;

}



/*************************************************************************************************************************************
  @name         :
  @description  : Checks if an individual has reached his life expectancy. Handle death scenario and counters.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::checkDeath( quint64 age )
{
    QSharedPointer<GWSAgent> agent = this->getAgent();

    quint64 testAge = 0;
    bool died = false;

    int min = globalParam.life_expectancy - globalParam.life_expectancy_margin;
    int max = globalParam.life_expectancy + globalParam.life_expectancy_margin;
    testAge = this->generateRandom( min, max );

    if ( age > testAge )
        {
        agent->setProperty( GWSExecutionEnvironment::DEATH_PROP , GWSTimeEnvironment::globalInstance()->getCurrentDateTime());
        died = true;
        }
     else
        {
        died == false;
        }

    return TRUE;
}



/*************************************************************************************************************************************
  @name         :
  @description  : Checks if an individual has reached his life expectency. Handle death scenario and counters. Keep the individual in
                  the couple list as long as both of them have not died.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::checkCoupleDeath()
{
    QList<couple_t>::iterator it;
    int testAge = 0;
    bool died = false;
    int min = globalParam.life_expectancy - globalParam.life_expectancy_margin;
    int max = globalParam.life_expectancy + globalParam.life_expectancy_margin;

    if ( cpList.empty() )
    {
        //debug_print(("DEBUG: No person in list"));
        return EMPTY_LIST;
    }

    it = cpList.begin();

    while (it != cpList.end())
    {
        died = false;

        /* check for husband */
        testAge = this->generateRandom(min, max);
        if ((it->mDead == false) && (it->mAge >= testAge))
        {
            qDebug() << QString("# M-%1 died at the age of %2").arg( it->mId ).arg( it->mAge );
            it->mDead = true;
            it->isPartnerDead = true;
            globalStats.totalMaleAge = globalStats.totalMaleAge - it->mAge;
            globalStats.totalMales--;
            globalStats.totalMaleDeath++;
            globalStats.deathsThisYear++;

        }

        /* check for wife */
        testAge = this->generateRandom(min, max);
        if ((it->fDead == false) && (it->fAge >= testAge))
        {
            qDebug() << QString("# F-%1 died at the age of %2").arg( it->fId ).arg( it->fAge );
            it->fDead = true;
            it->isPartnerDead = true;
            globalStats.totalFemaleAge = globalStats.totalFemaleAge - it->fAge;
            globalStats.totalFemales--;
            globalStats.totalFemaleDeath++;
            globalStats.deathsThisYear++;

        }

        /* Couple dead */
        if (it->mDead && it->fDead)
        {
            globalStats.totalCouples--;
            if (globalStats.totalChildren >= it->children)
            {
                globalStats.totalChildren = globalStats.totalChildren - it->children;
            }
            cpList.erase(it++);
            died = true;
        }

        if (died == false)
        {
            it++;
        }
    }

    return SUCCESS;
}



/*************************************************************************************************************************************
  @name         :
  @description  : Check if a person is eligible for marriage. Choose a suitable partner. get them married and add to the couple list.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::checkMarriage()
{
    QList<person_t>::iterator it;
    QList<person_t>::iterator it2;
    bool married = false;

    if (mList.size() == 0)
    {
        //debug_print(("DEBUG: No person in couple list"));
        return SUCCESS;
    }

    /* Pick one male and check his eligibility */
    it = mList.begin();
    while (it != mList.end())
    {
        married = false;

        /* Check if he has reached his legal marriage age */
        if (it->age < globalParam.marry_ageMale)
        {
            it++;
            continue;
        }

        int testAge = 0;
        int min = globalParam.marry_ageMale;
        int max = globalParam.marry_ageMale + globalParam.marry_age_margin;
        testAge = this->generateRandom(min, max);

        if (it->age > testAge)
        {
            /* Suitable male candidate, now search a partner for him. */
            it2 = fList.begin();
            while (it2 != fList.end())
            {
                /* Check if she has reached his legal marriage age */
                if (it2->age < globalParam.marry_ageFemale)
                {
                    it2++;
                    continue;
                }

                int testAge2 = 0;
                int min2 = globalParam.marry_ageFemale;
                int max2 = globalParam.marry_ageFemale + globalParam.marry_age_margin;
                testAge2 = this->generateRandom(min2, max2);

                if (it2->age < testAge2)
                {
                    /* VOILA!, we found a couple */
                    person_t m = *it;
                    person_t f = *it2;

                    qDebug() << QString("# M-%1 [%2] marries to F-%3 [%4]").arg( it->id).arg( it->age).arg( it2->id ).arg(it2->age );

                    /* Remove from individual list */
                    mList.erase(it++);
                    fList.erase(it2++);
                    married = true;

                    /* Add to couple list */
                    this->addToCoupleList(m, f);
                    break;
                }

                if (married == false)
                {
                    it2++;
                }
            }
        }

        if ( married == false )
        {
            //debug_print(("No partner found for M-%d.", it->id));
            it++;
        }
    }
    return SUCCESS;
}




/*************************************************************************************************************************************
  @name         :
  @description  : Add the couple to couple list and initialize the data.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::addToCoupleList(person_t m, person_t f)
{
    couple_t couple;

    /* Initialize couple data */
    couple.mId = m.id;
    couple.fId = f.id;
    couple.mAge = m.age;
    couple.fAge = f.age;
    couple.children = 0;
    couple.isDormant = false;
    couple.lastChildDuration = 0;
    couple.isPartnerDead = false;
    couple.mDead = false;
    couple.fDead = false;

    /* Add to couple list */
    cpList.push_back(couple);

    globalStats.totalCouples++;

    return SUCCESS;
}



/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::checkBirth()
{
    QList<couple_t>::iterator it;

    if (cpList.empty())
    {
        //debug_print(("No couple present"));
        return FAILURE;
    }

    it = cpList.begin();
    globalStats.birthsThisYear = 0;

    while (it != cpList.end())
    {
        if (it->isDormant)
        {
            it++;
            //debug_print(("DEBUG: Cannot reproduce. Dormant."));
            continue;
        }

        if (it->lastChildDuration < globalParam.next_child_gap)
        {
            it++;
            //debug_print(("DEBUG: Cannot reproduce. Child gap not attained"));
            continue;
        }

        if (it->children >= globalParam.total_fertility_rate)
        {
            it++;
            //debug_print(("DEBUG: Cannot reproduce. TFR exceeded!"));
            continue;
        }

        int testAge = 0;
        int min = globalParam.marry_ageFemale;
        int max = globalParam.max_female_fertility_age ;
        testAge = this->generateRandom(min, max);

        if ( it->fAge > testAge )
        {
            /* Reproduce */
            int testSample = 0;
            int min = 1;
            int max = 10 ;
            testSample = this->generateRandom(min, max);

            if ( testSample <= 5 )
            {
                /* Baby girl */
                person_t f;
                globalStats.fIndex++;
                globalStats.totalFemales++;
                f.age = 0;
                f.id = globalStats.fIndex;
                fList.push_back(f);
                qDebug() << QString( "# M-%1 [%2] and F-%3 [%4] gave birth to a baby girl F-%5" ).arg( it->mId ).arg( it->mAge ).arg( it->fId ).arg( it->fAge ).arg( f.id );


                // MAIALEN: write added people to .csv file
                QFile csv( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/PopulationGenerator/census.csv" );
                if( csv.open( QIODevice::ReadWrite ) ) {
                       QTextStream stream(&csv);
                       stream << f.id <<";" << f.age << ";" << 101 << endl;
                   }
                csv.close();
            }
            else
            {
                /* Baby boy */
                person_t m;
                globalStats.mIndex++;
                globalStats.totalMales++;
                m.age = 0;
                m.id = globalStats.mIndex;
                mList.push_back(m);
                qDebug() << QString( "# M-%1 [%2] and F-%3 [%4] gave birth to a baby boy M-%5" ).arg( it->mId ).arg( it->mAge ).arg( it->fId ).arg( it->fAge ).arg( m.id );

                // MAIALEN: write added people to .csv file
                QFile csv( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/PopulationGenerator/census.csv" );
                if( csv.open( QIODevice::ReadWrite ) ) {
                       QTextStream stream(&csv);
                       stream << m.id <<";" << m.age << ";" << 100 << endl;
                   }
                csv.close();



            }

            globalStats.birthsThisYear++;





            /* Update couple details */
            it->children++;
            globalStats.totalChildren++;
            it->lastChildDuration = 0;
            if ( it->children >= globalParam.total_fertility_rate )
            {
                it->isDormant = true;
            }



        }

        it++;
    }

    return SUCCESS;
}




/*************************************************************************************************************************************
  @name         :
  @description  : Simulates advancement of 1 year. Following are simulated:-
                    1) Ageing
                    2) Marriage
                    3) Birth
                    4) Death
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::simulate(int yearsToAdvance)
{
    int i = 0;
    int retval = 0;

    qDebug() << "Processing...";

    while ( i < yearsToAdvance )
    {
        /* Check current status of population */
        int categoryCount = 0;
        if ( ( mList.empty() ) || ( globalStats.totalMales == 0 ) )
        {
            //debug_print(("# No males present"));
            categoryCount++;
        }
        if ( ( fList.empty() ) || ( globalStats.totalFemales == 0 ) )
        {
            //debug_print(("# No females present"));
            categoryCount++;
        }
        if ( ( cpList.empty() ) || ( globalStats.totalCouples == 0) )
        {
            categoryCount++;
        }

        if ( categoryCount >= 3 )
        {
            qDebug() << "# Population Wiped out.";

            /* Update elapsed time counter */
            globalStats.totalYears++;
            i++;
            continue;
        }

        /* Advance age of males by 1 year */
        if ( globalStats.totalMales >= 1 )
        {
            retval = this->updateIndividualList( &mList, MALE );
        }

        /* Advance age of females by 1 year */
        if ( globalStats.totalFemales >= 1 )
        {
            retval = this->updateIndividualList( &fList, FEMALE );
        }

        /* Advance age of couples by 1 year */
        if ( globalStats.totalCouples >= 1 )
        {
            retval = this->updateCoupleList();
        }

        /* Check Male death */
        if ( globalStats.totalMales >= 1 )
        {
            retval = this->checkDeath( &mList, MALE );
        }

        /* Check female death */
        if ( globalStats.totalFemales >= 1 )
        {
            retval = this->checkDeath( &fList, FEMALE );
        }

        /* Check death in couple */
        if ( globalStats.totalCouples >= 1 )
        {
            retval = this->checkCoupleDeath();
        }

        /* Marriage Eligiblity */
        if ( (globalStats.totalMales >= 1 ) && ( globalStats.totalFemales >= 1) )
        {
            retval = this->checkMarriage();
        }

        /* Birth check */
        if ( globalStats.totalCouples >= 1 )
        {
            retval = this->checkBirth();
        }

        /* Update elapsed time counter */
        globalStats.totalYears++;

        i++;
    }

    return SUCCESS;
}



/**********************************************************************
@name           :
@description    :
@param
@return
**********************************************************************/
unsigned int GeneratePopulationBehaviour::generateRandom(int min, int max)
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

