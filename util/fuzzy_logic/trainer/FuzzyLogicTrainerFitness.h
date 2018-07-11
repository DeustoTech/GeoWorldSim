#ifndef FUZZYLOGICTRAINERFITNESS_H
#define FUZZYLOGICTRAINERFITNESS_H

#include <QObject>

#include "utils/fuzzy_logic/FuzzyLogic.h"

QT_FORWARD_DECLARE_CLASS(FuzzyLogicTrainer)

/**
 * @brief The FuzzyLogicTrainerFitness class
 *
 * Independent class to be used inside QRunnables thread and be faster
 */

class FuzzyLogicTrainerFitness : public QObject
{
    Q_OBJECT

public:
    explicit FuzzyLogicTrainerFitness( FuzzyLogicTrainer* trainer , QObject* parent = Q_NULLPTR);

    // FITNESS
    virtual double calculateFitness( QList< FuzzyLogic::InputMemberShipFunction > inputset , QList< FuzzyLogic::Rule > ruleset , FuzzyLogic::OutputMembershipFunction output , QString accumulator , QString conjunction , QString disjunction , QString activator ) = 0; // IMPORTANT!! The greater, the better

protected:
    FuzzyLogicTrainer* trainer;

};

#endif // FUZZYLOGICTRAINERFITNESS_H
