#ifndef GENERATEPOPULATIONBEHAVIOUR_H
#define GENERATEPOPULATIONBEHAVIOUR_H

#include "../Behaviour.h"
#include<iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <string.h>

/* Comment below line to switch OFF debugging */
#define DEBUG 1

#define SUCCESS 0
#define FAILURE 1
#define MALE 100
#define FEMALE 101
#define TRUE 1
#define FALSE 0
#define EMPTY_LIST 99

class GeneratePopulationBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GeneratePopulationBehaviour();

    // PROPERTIES
    static QString SIMULATION_LENGTH_YEARS;
    static QString NEXT;
    /* GLOBALS */

    /* Parameters */
    typedef struct globalParam_tag
    {
        int marryAgeMale;
        int marryAgeFemale;
        int marryAgeMargin;
        int lifeExpectency;
        int lifeExpectencyMargin;
        int nextChildGap;
        int tfr; /* Total Fertility Rate */
        int maxMaleFertilityAge;
        int maxFemaleFertilityAge;
    }globalParam_t;

    /* Stats */
    typedef struct globalStats_tag
    {
        unsigned int mIndex;
        unsigned int fIndex;
        unsigned int totalMales;
        unsigned int totalFemales;
        unsigned int totalCouples;
        unsigned int totalMaleAge;
        unsigned int totalFemaleAge;
        unsigned int totalMaleDeath;
        unsigned int totalFemaleDeath;
        unsigned int totalYears;
        unsigned int birthsThisYear;
        unsigned int deathsThisYear;
        unsigned int totalChildren;
    }globalStats_t;

    typedef struct person_tag
    {
        unsigned id;
        int age;
    }person_t;

    typedef struct couple_tag
    {
        unsigned mId;
        unsigned fId;
        int mAge;
        int fAge;
        bool mDead;
        bool fDead;
        int children;
        int lastChildDuration;
        bool isDormant;
        bool isPartnerDead;
    }couple_t;

    globalParam_t globalParam;
    globalStats_t globalStats;
    QList<person_t> mList;
    QList<person_t> fList;
    QList<couple_t> cpList;

private slots:
    virtual QJsonArray behave();

protected:
    virtual int launch();
    virtual int addPerson( int sex, person_t person );
    virtual int updateIndividualList( QList<person_t> *lst, int sex );
    virtual int updateCoupleList();
    virtual int checkDeath(QList<person_t> *lst, int sex);
    virtual int checkCoupleDeath();
    virtual int checkMarriage();
    virtual int addToCoupleList(person_t m, person_t f);
    virtual int checkBirth();
    virtual int simulate(int yearsToAdvance);
    unsigned int generateRandom(int min, int max);
    QFile csv;
};

#endif // GENERATEPOPULATIONBEHAVIOUR_H
