#ifndef RULECROSSOVEROPERATOR_H
#define RULECROSSOVEROPERATOR_H

#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainerOperator.h"

class RuleCrossoverOperator : public FuzzyLogicTrainerOperator
{
    Q_OBJECT
public:

    explicit RuleCrossoverOperator( FuzzyLogicTrainer *trainer ) : FuzzyLogicTrainerOperator( trainer ){}
    virtual RuleCrossoverOperator* clone(FuzzyLogicTrainer *trainer) = 0;

    void init( QList< FuzzyLogic::Rule > ruleset1_to_crossover ,QList< FuzzyLogic::Rule > ruleset2_to_crossover  ){
        this->ruleset1_to_crossover = ruleset1_to_crossover;
        this->ruleset2_to_crossover = ruleset2_to_crossover;
    }

    void run() override {

        this->obtained_ruleset = this->crossover( this->ruleset1_to_crossover , this->ruleset2_to_crossover );

        // Compare the new candidate ruleset with the best
        FuzzyLogicTrainer::FuzzyExperiment exp = this->trainer->getBestExperiment();
        this->obtained_fitness = this->trainer->calculateFitness( exp.inputset , this->obtained_ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator );
    }

    QList<FuzzyLogic::Rule> getObtainedRuleset(){ return this->obtained_ruleset; }

private:

    virtual QList< FuzzyLogic::Rule > crossover( QList< FuzzyLogic::Rule > rs1 , QList< FuzzyLogic::Rule > rs2 ) = 0;
    QList< FuzzyLogic::Rule > ruleset1_to_crossover, ruleset2_to_crossover;
    QList<FuzzyLogic::Rule> obtained_ruleset;

};

#endif // RULECROSSOVEROPERATOR_H
