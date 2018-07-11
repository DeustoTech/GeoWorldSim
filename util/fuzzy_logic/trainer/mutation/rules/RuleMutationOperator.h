#ifndef RULEMUTATIONOPERATOR_H
#define RULEMUTATIONOPERATOR_H

#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainerOperator.h"

class RuleMutationOperator : public FuzzyLogicTrainerOperator
{
    Q_OBJECT
public:

    explicit RuleMutationOperator( FuzzyLogicTrainer *trainer ) : FuzzyLogicTrainerOperator( trainer ){}

    void init( QList< FuzzyLogic::Rule > ruleset_to_mutate ){
        this->ruleset_to_mutate = ruleset_to_mutate;
    }

    void run() override {

        this->obtained_ruleset = this->mutation( this->ruleset_to_mutate );

        // Compare the new candidate ruleset with the best
        FuzzyLogicTrainer::FuzzyExperiment exp = this->trainer->getBestExperiment();
        this->obtained_fitness = this->trainer->calculateFitness( exp.inputset , this->obtained_ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator );
    }

    QList<FuzzyLogic::Rule> getObtainedRuleset(){ return this->obtained_ruleset; }

private:
    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs) = 0;
    QList< FuzzyLogic::Rule > ruleset_to_mutate;
    QList<FuzzyLogic::Rule> obtained_ruleset;

};

#endif // RULEMUTATIONOPERATOR_H
