#include "FuzzyLogicTrainerFitness.h"

#include "utils/fuzzy_logic/trainer/FuzzyLogicTrainer.h"

FuzzyLogicTrainerFitness::FuzzyLogicTrainerFitness(FuzzyLogicTrainer* trainer , QObject* parent) : QObject( trainer ){
    this->trainer = trainer;
}

