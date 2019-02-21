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


class GeneratePopulationBehaviour : public GWSBehaviour
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit GeneratePopulationBehaviour();

    // PROPERTIES
    static QString LOOKING_FOR;
    static QString COUPLE_ID;
    static QString MARRY_AGE;
    static QString MARRY_AGE_MARGIN;
    static QString LIFE_EXPECTANCY;
    static QString LIFE_EXPECTANCY_MARGIN;
    static QString NEXT_CHILD_GAP;
    static QString TOTAL_FERTILITY_RATE;
    static QString MAX_FERTILITY_AGE;
    static QString CHILDREN_IDS;
    static QString SIMULATION_LENGTH_YEARS;
    static QString PARENT1;
    static QString PARENT2;
    static QString NEXT;

private slots:
    virtual QJsonArray behave();

protected:
    virtual bool checkDeath( int age );
    virtual bool checkMarriage( int age );
    virtual bool checkBirth( int age );
    virtual void initialize();
    int getAge( QSharedPointer<GWSAgent> agent );
    double generateRandom(int min, int max);
};

#endif // GENERATEPOPULATIONBEHAVIOUR_H
