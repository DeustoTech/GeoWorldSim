#ifndef RULEADDMUTATION
#define RULEADDMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleAddMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleAddMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){};

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        FuzzyLogic::Rule rule = this->trainer->generateRule();
        result.append( rule );

        return result;
    }
};

#endif // RULEADDMUTATION

