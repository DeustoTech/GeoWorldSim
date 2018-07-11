#ifndef INPUTCOMBINECROSSOVER
#define INPUTCOMBINECROSSOVER

#include "utils/fuzzy_logic/trainer/crossover/inputs/InputCrossoverOperator.h"

class InputCombineCrossover : public InputCrossoverOperator
{
    Q_OBJECT

public:
    InputCombineCrossover(FuzzyLogicTrainer *parent) : InputCrossoverOperator(parent){}
    virtual InputCombineCrossover* clone(FuzzyLogicTrainer *trainer){ return new InputCombineCrossover( trainer ); }

    virtual QList< FuzzyLogic::InputMemberShipFunction > crossover( QList< FuzzyLogic::InputMemberShipFunction > is1 , QList< FuzzyLogic::InputMemberShipFunction > is2 ){
        QList< FuzzyLogic::InputMemberShipFunction > result;

        int max_size = qMax( is1.size() , is2.size() );

        for( int i = 0 ; i < max_size ; i++ ){
            int distr = qrand() % 2;
            if( distr < 1 && i < is1.size() ){
                result.append( is1.at( i ) );
            }
            if( distr >= 1 && i < is2.size() ){
                result.append( is2.at( i ) );
            }
        }
        return result;
    }
};


#endif // INPUTCOMBINECROSSOVER

