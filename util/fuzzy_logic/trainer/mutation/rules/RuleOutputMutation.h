#ifndef RULEOUTPUTMUTATION
#define RULEOUTPUTMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleOutputMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleOutputMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){}

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        int rule_pos = qrand() % result.size();

        QStringList output_names = this->trainer->getOutputNames();
        result[ rule_pos ].output_value = output_names.at( qrand() % output_names.size() );

        return result;
    }
};



#endif // RULEOUTPUTMUTATION

