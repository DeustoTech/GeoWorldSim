#ifndef INPUTSHAPEMUTATION
#define INPUTSHAPEMUTATION

#include "utils/fuzzy_logic/trainer/mutation/inputs/InputMutationOperator.h"

class InputShapeMutation : public InputMutationOperator
{
    Q_OBJECT

public:
    InputShapeMutation(FuzzyLogicTrainer *parent) : InputMutationOperator(parent){}
    virtual InputShapeMutation* clone(FuzzyLogicTrainer *trainer){ return new InputShapeMutation( trainer ); }

    virtual QList< FuzzyLogic::InputMemberShipFunction > mutation(QList<FuzzyLogic::InputMemberShipFunction> is){
        QList<FuzzyLogic::InputMemberShipFunction> result = is;

        int input_pos = qrand() % result.size();

        // AVOIG GETTING TERM 0 AND LAST
        int term_pos = qrand() % ( result.at( input_pos ).terms.size()-2 ) + 1;

        // Get left triangle center point
        double left_bound = result.at( input_pos ).terms.at( term_pos ).shape_points.at( 0 );
        // Get left triangle center point
        double right_bound = result.at( input_pos ).terms.at( term_pos ).shape_points.at( 2 );

        // The term that is being changed's center must be between the two bounds
        double center = ((qrand() % qMax( 1 , int((right_bound - left_bound)*10000))) + left_bound*10000) / 10000.0;

        result[ input_pos ].terms[ term_pos-1 ].shape_points[ 2 ] = center;
        result[ input_pos ].terms[ term_pos+1 ].shape_points[ 0 ] = center;
        result[ input_pos ].terms[ term_pos ].shape_points[ 1 ] = center;

        return result;
    }
};



#endif // INPUTSHAPEMUTATION

