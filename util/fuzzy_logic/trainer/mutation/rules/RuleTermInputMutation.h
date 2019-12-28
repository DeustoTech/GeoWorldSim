#ifndef RULETERMINPUTMUTATION
#define RULETERMINPUTMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleTermInputMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleTermInputMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){}

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        int rule_pos = qrand() % result.size();
        // Select term pos to be changed
        int term_pos = qrand() % result[ rule_pos ].terms.size();
        QStringList input_names = this->trainer->getInputNames().keys();
        result[ rule_pos ].terms[ term_pos ].input_name = input_names.at( qrand() % input_names.size() );

        return result;
    }
};


#endif // RULETERMINPUTMUTATION

