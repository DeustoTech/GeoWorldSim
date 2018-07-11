#ifndef RULESLICECROSSOVER_H
#define RULESLICECROSSOVER_H

#include "utils/fuzzy_logic/trainer/crossover/rules/RuleCrossoverOperator.h"

class RuleSliceCrossover : public RuleCrossoverOperator
{
    Q_OBJECT

public:
    RuleSliceCrossover(FuzzyLogicTrainer *parent) : RuleCrossoverOperator(parent){}
    virtual RuleSliceCrossover* clone(FuzzyLogicTrainer *trainer){ return new RuleSliceCrossover( trainer ); }

    virtual QList< FuzzyLogic::Rule > crossover( QList< FuzzyLogic::Rule > rs1 , QList< FuzzyLogic::Rule > rs2 ){
        QList< FuzzyLogic::Rule > result;

        // Get minimum size of both
        int min_size = qMin( rs1.size() , rs2.size() );
        int slice_position = qrand() % min_size;

        QList< FuzzyLogic::Rule > take_out_rs1 = rs1.mid( 0 , slice_position );
        QList< FuzzyLogic::Rule > take_out_rs2 = rs2.mid( slice_position );

        result.append( take_out_rs1 );
        result.append( take_out_rs2 );

        return result;
    }
};

#endif // RULESLICECROSSOVER_H
