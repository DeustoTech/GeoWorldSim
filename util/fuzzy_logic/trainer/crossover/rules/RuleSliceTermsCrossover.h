#ifndef RULESLICETERMSCROSSOVER
#define RULESLICETERMSCROSSOVER

#include "utils/fuzzy_logic/trainer/crossover/rules/RuleCrossoverOperator.h"

class RuleSliceTermsCrossover : public RuleCrossoverOperator
{
    Q_OBJECT

public:
    RuleSliceTermsCrossover(FuzzyLogicTrainer *parent) : RuleCrossoverOperator(parent){}
    virtual RuleSliceTermsCrossover* clone(FuzzyLogicTrainer *trainer){ return new RuleSliceTermsCrossover( trainer ); }

    virtual QList< FuzzyLogic::Rule > crossover( QList< FuzzyLogic::Rule > rs1 , QList< FuzzyLogic::Rule > rs2 ){
        QList< FuzzyLogic::Rule > result = rs1;

        int rule_pos = qrand() % qMin( rs1.size(), rs2.size() );
        int slice_position = qrand() % qMin( rs1.at( rule_pos ).terms.size() , rs2.at( rule_pos ).terms.size() );

        QList< FuzzyLogic::RuleTerm > take_out_rs1 = rs1.at( rule_pos ).terms.mid( 0 , slice_position );
        QList< FuzzyLogic::RuleTerm > take_out_rs2 = rs2.at( rule_pos ).terms.mid( slice_position );

        result[ rule_pos ].terms.clear();
        result[ rule_pos ].terms.append( take_out_rs1 );
        result[ rule_pos ].terms.append( take_out_rs2 );

        return result;
    }
};

#endif // RULESLICETERMSCROSSOVER

