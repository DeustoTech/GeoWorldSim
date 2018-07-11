#ifndef RULETERMHEDGEMUTATION
#define RULETERMHEDGEMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleTermHedgeMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleTermHedgeMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){}

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        int rule_pos = qrand() % result.size();
        // Select term pos to be changed
        int term_pos = qrand() % result[ rule_pos ].terms.size();

        QStringList hedges; hedges << "" << "not" << "somewhat" << "seldom" << "very" << "extremely";
        result[ rule_pos ].terms[ term_pos ].hedge = hedges.at( qrand() % hedges.size() );

        return result;
    }
};

#endif // RULETERMHEDGEMUTATION

