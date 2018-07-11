#ifndef RULEREMOVEMUTATION
#define RULEREMOVEMUTATION

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"

class RuleRemoveMutation : public RuleMutationOperator
{
    Q_OBJECT

public:
    RuleRemoveMutation( FuzzyLogicTrainer *trainer ) : RuleMutationOperator( trainer ){}

    virtual QList< FuzzyLogic::Rule > mutation(QList<FuzzyLogic::Rule> rs){
        QList<FuzzyLogic::Rule> result = rs;

        if( result.size() > 1 ){
            result.removeAt( qrand() % result.size() );
        }

        return result;
    }
};


#endif // RULEREMOVEMUTATION

