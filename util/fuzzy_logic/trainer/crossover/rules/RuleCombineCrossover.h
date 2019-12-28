#ifndef RULECOMBINECROSSOVER
#define RULECOMBINECROSSOVER

#include "utils/fuzzy_logic/trainer/crossover/rules/RuleCrossoverOperator.h"

class RuleCombineCrossover : public RuleCrossoverOperator
{
    Q_OBJECT

public:
    RuleCombineCrossover(FuzzyLogicTrainer *parent) : RuleCrossoverOperator(parent){}
    virtual RuleCombineCrossover* clone(FuzzyLogicTrainer *trainer){ return new RuleCombineCrossover( trainer ); }

    virtual QList< FuzzyLogic::Rule > crossover( QList< FuzzyLogic::Rule > rs1 , QList< FuzzyLogic::Rule > rs2 ){
        QList< FuzzyLogic::Rule > result;

        int max_size = qMax( rs1.size() , rs2.size() );

        for( int i = 0 ; i < max_size ; i++ ){
            int distr = qrand() % 2;
            if( distr < 1 && i < rs1.size() ){
                result.append( rs1.at( i ) );
            }
            if( distr >= 1 && i < rs2.size() ){
                result.append( rs2.at( i ) );
            }
        }
        return result;
    }
};


#endif // RULECOMBINECROSSOVER

