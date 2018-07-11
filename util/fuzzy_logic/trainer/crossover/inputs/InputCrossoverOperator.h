#ifndef INPUTCROSSOVEROPERATOR_H
#define INPUTCROSSOVEROPERATOR_H

#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainerOperator.h"

class InputCrossoverOperator : public FuzzyLogicTrainerOperator
{
    Q_OBJECT
public:

    // Constructors
    explicit InputCrossoverOperator(FuzzyLogicTrainer *trainer) : FuzzyLogicTrainerOperator( trainer ){}
    virtual InputCrossoverOperator* clone(FuzzyLogicTrainer *trainer) = 0;

    void init( QList< FuzzyLogic::InputMemberShipFunction > inputset1_to_crossover ,QList< FuzzyLogic::InputMemberShipFunction > inputset2_to_crossover ){
        this->inputset1_to_crossover = inputset1_to_crossover;
        this->inputset2_to_crossover = inputset2_to_crossover;
    }

    void run() override {

        this->obtained_inputset = this->crossover( this->inputset1_to_crossover , this->inputset2_to_crossover );

        // Compare the new candidate ruleset with the best
        FuzzyLogicTrainer::FuzzyExperiment exp = this->trainer->getBestExperiment();
        this->obtained_fitness = this->trainer->calculateFitness( this->obtained_inputset , exp.ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator );
    }

    QList<FuzzyLogic::InputMemberShipFunction> getObtainedInputset(){ return this->obtained_inputset; }

private:
    virtual QList< FuzzyLogic::InputMemberShipFunction > crossover( QList< FuzzyLogic::InputMemberShipFunction > is1 , QList< FuzzyLogic::InputMemberShipFunction > is2 ) = 0;
    QList< FuzzyLogic::InputMemberShipFunction > inputset1_to_crossover, inputset2_to_crossover;
    QList<FuzzyLogic::InputMemberShipFunction> obtained_inputset;
};

#endif // INPUTCROSSOVEROPERATOR_H
