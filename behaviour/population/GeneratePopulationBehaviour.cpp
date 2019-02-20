#include "GeneratePopulationBehaviour.h"
#include <QFile>
#include <QTextStream>


QString GeneratePopulationBehaviour::SIMULATION_LENGTH_YEARS = "years_to_simulate";
QString GeneratePopulationBehaviour::INITIAL_MALE_AGE = "set_first_males_age";
QString GeneratePopulationBehaviour::INITIAL_FEMALE_AGE = "set_first_females_age";
QString GeneratePopulationBehaviour::NEXT = "next";

GeneratePopulationBehaviour::GeneratePopulationBehaviour() : GWSBehaviour()
{

}


/**********************************************************************
 SLOTS
**********************************************************************/

QJsonArray GeneratePopulationBehaviour::behave(){

    int retval = 0;
    int yearsToAdvance = this->getProperty( SIMULATION_LENGTH_YEARS ).toInt();

    /* Initialize global parameters */
    retval = this->launch();
    retval = this->simulate(yearsToAdvance);

    if ( retval != SUCCESS )
    {
       qDebug() << "ERROR: Could not simulate!";
    }


    return this->getProperty( NEXT ).toArray();
}


int GeneratePopulationBehaviour::launch(){

    /* Parameters */
    globalParam.marryAgeMale = 21;
    globalParam.marryAgeFemale = 18;
    globalParam.marryAgeMargin = 15;
    globalParam.lifeExpectency = 85;
    globalParam.lifeExpectencyMargin = 20;
    globalParam.nextChildGap = 3;
    globalParam.tfr = 6;
    globalParam.maxMaleFertilityAge = 60;
    globalParam.maxFemaleFertilityAge = 45;

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

    qDebug() << "Example" << globalParam.marryAgeMale;

    return SUCCESS;
}


/*************************************************************************************************************************************
  @name         :
  @description  :
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::addPerson(int sex, person_t person)
{
    int retval = 0;

    if (sex == MALE)
    {
        mList.push_back(person);
        globalStats.mIndex++;
        globalStats.totalMales++;
        globalStats.totalMaleAge =+ person.age;
    }
    else
    {
        fList.push_back(person);
        globalStats.fIndex++;
        globalStats.totalFemales++;
        globalStats.totalFemaleAge =+ person.age;
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
  @description  : Updates Male/Female list by 1 year advancement.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::updateIndividualList( QList<person_t> *lst, int sex )
{
    QList<person_t>::iterator it;

    if ( (*lst).size() == 0 )
    {
        //debug_print(("DEBUG: %s No person in list", __FUNCTION__));
        return SUCCESS;
    }

    it = (*lst).begin();

    while ( it != (*lst).end() )
    {
        ( it->age )++;

        /* Update Stats */
        if ( sex == MALE )
        {
            globalStats.totalMaleAge++;
        }
        else
        {
            globalStats.totalFemaleAge++;
        }
        it++;
    }

    return SUCCESS;
}



/*************************************************************************************************************************************
  @name         :
  @description  : Updates Male/Female list by 1 year advancement.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::updateCoupleList(){

    QList<couple_t>::iterator it;

    if ( cpList.size() == 0 )
    {
        //debug_print(("DEBUG: %s No person in couple list", __FUNCTION__));
        return SUCCESS;
    }

    it = cpList.begin();

    while ( it != cpList.end() )
    {
        if (!it->mDead)
        {
            ( it->mAge )++;
            globalStats.totalMaleAge++;
        }

        if (!it->fDead)
        {
            (it->fAge)++;
            globalStats.totalFemaleAge++;
        }

        (it->lastChildDuration)++;

        if ( (it->fAge > globalParam.maxFemaleFertilityAge) || (it->mAge > globalParam.maxMaleFertilityAge) )
        {
            it->isDormant = true;
        }

        it++;
    }

    return SUCCESS;
}



/*************************************************************************************************************************************
  @name         :
  @description  : Checks if an individual has reached his life expectency. Handle death scenario and counters.
  @param
  @return
*************************************************************************************************************************************/
int GeneratePopulationBehaviour::checkDeath( QList<person_t> *lst, int sex )
{
    QList<person_t>::iterator it;
    int testAge = 0;
    bool died = false;
    int min = globalParam.lifeExpectency - globalParam.lifeExpectencyMargin;
    int max = globalParam.lifeExpectency + globalParam.lifeExpectencyMargin;

    globalStats.deathsThisYear = 0;

    if ((*lst).size() == 0)
    {
        //debug_print(("DEBUG: No person in list"));
        return EMPTY_LIST;
    }

    it = (*lst).begin();

    while (it != (*lst).end())
    {
        died = false;
        testAge = this->generateRandom(min, max);
        if (it->age > testAge)
        {
            /* Update Stats. We need the data of the person to update stats.
               So clearing is done after this.*/
            if (sex == MALE)
            {
                qDebug() << QString("# M-%1 died at the age of %2").arg( it->id).arg( it->age);
                globalStats.totalMaleAge = globalStats.totalMaleAge - it->age;
                globalStats.totalMales--;
                globalStats.totalMaleDeath++;
                globalStats.deathsThisYear++;
                mList.erase(it++);
                died = true;
            }
            else
            {
                qDebug() << QString("# F-%1 died at the age of %2").arg( it->id).arg( it->age);
                globalStats.totalFemaleAge = globalStats.totalFemaleAge - it->age;
                globalStats.totalFemales--;
                globalStats.totalFemaleDeath++;
                globalStats.deathsThisYear++;
                fList.erase(it++);
                died = true;

            }
            // MAIALEN: write added people to .csv file
            QFile csv( "/home/maialen/Escritorio/WorkSpace/GeoWorldSim/usecase/PopulationGenerator/census.csv" );

            if( csv.open( QIODevice::ReadWrite ) ) {
                   QTextStream stream(&csv);
                   stream << it->id <<";" << it->age << ";" << sex << endl;
               }
            csv.close();


        }

        if (died == false)
        {
            it++;
        }
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
    int min = globalParam.lifeExpectency - globalParam.lifeExpectencyMargin;
    int max = globalParam.lifeExpectency + globalParam.lifeExpectencyMargin;

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
        if (it->age < globalParam.marryAgeMale)
        {
            it++;
            continue;
        }

        int testAge = 0;
        int min = globalParam.marryAgeMale;
        int max = globalParam.marryAgeMale + globalParam.marryAgeMargin;
        testAge = this->generateRandom(min, max);

        if (it->age > testAge)
        {
            /* Suitable male candidate, now search a partner for him. */
            it2 = fList.begin();
            while (it2 != fList.end())
            {
                /* Check if she has reached his legal marriage age */
                if (it2->age < globalParam.marryAgeFemale)
                {
                    it2++;
                    continue;
                }

                int testAge2 = 0;
                int min2 = globalParam.marryAgeFemale;
                int max2 = globalParam.marryAgeFemale + globalParam.marryAgeMargin;
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

        if (it->lastChildDuration < globalParam.nextChildGap)
        {
            it++;
            //debug_print(("DEBUG: Cannot reproduce. Child gap not attained"));
            continue;
        }

        if (it->children >= globalParam.tfr)
        {
            it++;
            //debug_print(("DEBUG: Cannot reproduce. TFR exceeded!"));
            continue;
        }

        int testAge = 0;
        int min = globalParam.marryAgeFemale;
        int max = globalParam.maxFemaleFertilityAge ;
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
            if ( it->children >= globalParam.tfr )
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

