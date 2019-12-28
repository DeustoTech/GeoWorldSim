#ifndef INPUTMUTATIONOPERATOR_H
#define INPUTMUTATIONOPERATOR_H

#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainerOperator.h"

class InputMutationOperator : public FuzzyLogicTrainerOperator
{
    Q_OBJECT
public:

    explicit InputMutationOperator(FuzzyLogicTrainer *trainer) : FuzzyLogicTrainerOperator( trainer ){}
    virtual InputMutationOperator* clone(FuzzyLogicTrainer *trainer) = 0;

    void init( QList< FuzzyLogic::InputMemberShipFunction > inputset_to_mutate ){
        this->inputset_to_mutate = inputset_to_mutate;
    }

    void run() override {

        this->obtained_inputset = this->mutation( this->inputset_to_mutate );

        // Compare the new candidate ruleset with the best
        FuzzyLogicTrainer::FuzzyExperiment exp = this->trainer->getBestExperiment();
        this->obtained_fitness = this->trainer->calculateFitness( this->obtained_inputset , exp.ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator );
    }

    QList<FuzzyLogic::InputMemberShipFunction> getObtainedInputset(){ return this->obtained_inputset; }

private:
    virtual QList< FuzzyLogic::InputMemberShipFunction > mutation( QList< FuzzyLogic::InputMemberShipFunction > i ) = 0;
    QList< FuzzyLogic::InputMemberShipFunction > inputset_to_mutate;
    QList<FuzzyLogic::InputMemberShipFunction> obtained_inputset;

};

#endif // INPUTMUTATIONOPERATOR_H
