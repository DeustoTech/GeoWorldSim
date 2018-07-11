#ifndef RULEREMOVETERMMUTATION
#define RULEREMOVETERMMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleRemoveTermMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleRemoveTermMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){}

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        int rule_pos = qrand() % rs.size();
        if( result[ rule_pos ].terms.size() < 2 ){ return result; }

        // Remove one
        result[ rule_pos ].terms.removeAt( qrand() % rs[ rule_pos ].terms.size() );

        return result;
    }
};

#endif // RULEREMOVETERMMUTATION

