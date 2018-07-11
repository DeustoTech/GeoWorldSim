#ifndef FUZZYLOGICTRAINEROPERATOR
#define FUZZYLOGICTRAINEROPERATOR

#include <QRunnable>
#include "utils/fuzzy_logic/FuzzyLogic.h"
#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainer.h"

class FuzzyLogicTrainerOperator : public QObject, public QRunnable
{
    Q_OBJECT
public:

    // Constructors
    explicit FuzzyLogicTrainerOperator(FuzzyLogicTrainer *trainer) { this->trainer = trainer; }
    virtual FuzzyLogicTrainerOperator* clone(FuzzyLogicTrainer *trainer) = 0;

    // Operations
    virtual void run() = 0;

    double getObtainedFitness(){ return this->obtained_fitness; }

protected:
    FuzzyLogicTrainer* trainer;
    double obtained_fitness = 0;

};

#endif // FUZZYLOGICTRAINEROPERATOR

