#include "FuzzyLogicTrainer.h"

#include <QDateTime>
#include <QProcess>
#include <QDir>
#include <QThreadPool>
#include <QDebug>

#include "utils/fuzzy_logic/trainer/crossover/rules/RuleCrossoverOperator.h"
#include "utils/fuzzy_logic/trainer/crossover/rules/RuleCombineCrossover.h"
#include "utils/fuzzy_logic/trainer/crossover/rules/RuleSliceCrossover.h"
#include "utils/fuzzy_logic/trainer/crossover/rules/RuleSliceTermsCrossover.h"
#include "utils/fuzzy_logic/trainer/crossover/inputs/InputCombineCrossover.h"

#include "utils/fuzzy_logic/trainer/mutation/rules/RuleMutationOperator.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleAddMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleAddTermMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleOutputMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleRemoveMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleRemoveTermMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleTermInputMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleTermHedgeMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/rules/RuleTermVariableMutation.h"
#include "utils/fuzzy_logic/trainer/mutation/inputs/InputShapeMutation.h"

#include "utils/parallelism_controller/ParallelismController.h"
#include "utils/sorters/NumberSorter.h"
#include "utils/statistics/Statistics.h"

FuzzyLogicTrainer::FuzzyLogicTrainer( QMap<QString , QStringList> input_values , QStringList outputs  , QObject *parent) : QObject(parent){

    this->accumulators << "AlgebraicSum" << "BoundedSum" << "DrasticSum" << "EinsteinSum" << "HamacherSum" << "Maximum" << "NormalizedSum";
    this->conjunctions << "AlgebraicProduct" << "BoundedDifference" << "DrasticProduct" << "EinsteinProduct" << "HamacherProduct" << "Minimum";
    this->disjunctions << "AlgebraicSum" << "BoundedSum" << "DrasticSum" << "EinsteinSum" << "HamacherSum" << "Maximum" << "NormalizedSum";
    this->activators << "AlgebraicProduct" << "BoundedDifference" << "DrasticProduct" << "EinsteinProduct" << "HamacherProduct" << "Minimum";

    // Load data structures for fast retrieve of values
    foreach( QString input , input_values.keys() ){

        this->input_names.insert( input , QStringList() );

        foreach( QString value_name , input_values.value( input ) ){
            this->input_names[ input ].append( value_name );
        }
    }
    this->output_names = outputs;
}

FuzzyLogicTrainer::~FuzzyLogicTrainer(){
    qDeleteAll( this->rule_operators );
    qDeleteAll( this->input_operators );
}

/**********************************************************************
   GETTERS
**********************************************************************/

QStringList FuzzyLogicTrainer::getOutputNames() const{
    return this->output_names;
}

QMap<QString , QStringList> FuzzyLogicTrainer::getInputNames() const{
    return this->input_names;
}

double FuzzyLogicTrainer::getBestFitness() const{
    return this->best_fitness;
}

FuzzyLogicTrainer::FuzzyExperiment FuzzyLogicTrainer::getBestExperiment() const{
    return this->best_experiment;
}

int FuzzyLogicTrainer::getIteration() const{
    return this->iteration;
}

int FuzzyLogicTrainer::getSubiteration() const{
    return this->subiteration;
}

/**********************************************************************
   GENERATORS
**********************************************************************/

QList< FuzzyLogic::InputMemberShipFunction > FuzzyLogicTrainer::generateInputset(){

    QList< FuzzyLogic::InputMemberShipFunction > inputs;

    foreach( QString input_name , this->input_names.keys() ){

        FuzzyLogic::InputMemberShipFunction input_mf;
        input_mf.input_name = input_name;

        // Create triangles randomly: calculate n-2 numbers (because the first will be 0 and last will be 1)
        QVector<double> randoms;
        srand( QDateTime::currentDateTime().toMSecsSinceEpoch() );
        randoms.append( 0 );
        for( int i = 0 ; i < this->input_names.value( input_name ).size() -2 ; i++){
            randoms.append( qrand() % 1000 / 1000.0 );
        }
        randoms.append( 1 );
        randoms = NumberSorter::sortAscending( randoms );

        for( int i = 0; i < this->input_names.value( input_name ).size() ; i++ ){

            QString value_name = this->input_names.value( input_name ).at( i );
            FuzzyLogic::MemberShipFunctionTerm term;
            term.name = value_name;

            // Center of the term will be at its position in randoms
            double center = randoms.at( i );

            // left point
            double left = i == 0 ? -0.25 : randoms.at( i-1 );
            // right point
            double right = i == randoms.size()-1 ? 1.25 : randoms.at( i+1 );

            term.shape_points.append( left );
            term.shape_points.append( center );
            term.shape_points.append( right );
            input_mf.terms.append( term );

        }
        inputs.append( input_mf );
    }

    return inputs;
}

QList< FuzzyLogic::Rule > FuzzyLogicTrainer::generateRuleset(){
    QList< FuzzyLogic::Rule > ruleset;
    for( int k = 0 ; k < (qrand() % 100)+1 ; k++ ){
        ruleset.append( this->generateRule() );
    }

    if( this->property("method").toString() == "EXPERT" ){
        QString expert_knowledge = "[HamacherSum,EinsteinProduct,HamacherSum,DrasticProduct] if (CAR_PRICE is  LOW)then CAR if (CAR_TIME is  LOW)(TRANSIT_TIME is  HIGH)then CAR if (CAR_TIME is  MEDIUM)(TRANSIT_TIME is  HIGH)then CAR if (CAR_TIME is  LOW)(MOTORCYCLE_TIME is  MEDIUM)then CAR if (CAR_TIME is  LOW)(MOTORCYCLE_TIME is  HIGH)then CAR if (CAR_PRICE is  LOW)(TRANSIT_PRICE is  LOW)then CAR if (CAR_PRICE is  LOW)(TRANSIT_PRICE is  MEDIUM)then CAR if (CAR_PRICE is  LOW)(TRANSIT_PRICE is  HIGH)then CAR if (CAR_PRICE is  LOW)(TRANSIT_PRICE is  HIGH)then CAR if (CAR_PRICE is  MEDIUM)(TRANSIT_PRICE is  MEDIUM)then CAR if (CAR_PRICE is  MEDIUM)(TRANSIT_PRICE is  HIGH)then CAR if (CAR_PRICE is  HIGH)(TRANSIT_PRICE is  HIGH)then CAR if (CAR_DISTANCE is  HIGH)(BICYCLE_DISTANCE is  HIGH)then CAR if (CAR_DISTANCE is  MEDIUM)(BICYCLE_DISTANCE is  HIGH)then CAR if (CAR_DISTANCE is  MEDIUM)(BICYCLE_DISTANCE is  HIGH)then CAR if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  LOW)then TRANSIT if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  MEDIUM)then TRANSIT if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  HIGH)then TRANSIT if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  MEDIUM)then TRANSIT if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  HIGH)then TRANSIT if (TRANSIT_PRICE is  MEDIUM)(CAR_PRICE is  HIGH)then TRANSIT if (TRANSIT_TIME is  LOW)(CAR_TIME is  LOW)then TRANSIT if (TRANSIT_PRICE is  LOW)(CAR_PRICE is  MEDIUM)then TRANSIT if (TRANSIT_TIME is  LOW)(CAR_TIME is  HIGH)then TRANSIT if (TRANSIT_PRICE is  MEDIUM)(CAR_PRICE is  MEDIUM)then TRANSIT if (TRANSIT_TIME is  MEDIUM)(CAR_TIME is  HIGH)then TRANSIT if (TRANSIT_TIME is  HIGH)(CAR_TIME is  HIGH)then TRANSIT if (MOTORCYCLE_DISTANCE is  LOW)(CAR_PRICE is  HIGH)then MOTORCYCLE if (MOTORCYCLE_PRICE is  MEDIUM)(CAR_PRICE is  HIGH)then MOTORCYCLE if (MOTORCYCLE_PRICE is  MEDIUM)(CAR_TIME is  HIGH)then MOTORCYCLE if (WALK_DISTANCE is  LOW)then WALK if (BICYCLE_DISTANCE is  LOW)then BICYCLE if (BICYCLE_TIME is  LOW)then BICYCLE [BICYCLE_DISTANCE:(-0.25 0 0.542)(0 0.542 1)(0.542 1 1.25)][BICYCLE_EMISSIONS:(-0.25 0 0.853)(0 0.853 1)(0.853 1 1.25)][BICYCLE_PRICE:(-0.25 0 0.419)(0 0.419 1)(0.419 1 1.25)][BICYCLE_TIME:(-0.25 0 0.952)(0 0.952 1)(0.952 1 1.25)][CAR_DISTANCE:(-0.25 0 0.284)(0 0.284 1)(0.284 1 1.25)][CAR_EMISSIONS:(-0.25 0 0.613)(0 0.613 1)(0.613 1 1.25)][CAR_PRICE:(-0.25 0 0.389)(0 0.389 1)(0.389 1 1.25)][CAR_TIME:(-0.25 0 0.334)(0 0.334 1)(0.334 1 1.25)][MOTORCYCLE_DISTANCE:(-0.25 0 0.789)(0 0.789 1)(0.789 1 1.25)][MOTORCYCLE_EMISSIONS:(-0.25 0 0.893)(0 0.893 1)(0.893 1 1.25)][MOTORCYCLE_PRICE:(-0.25 0 0.418)(0 0.418 1)(0.418 1 1.25)][MOTORCYCLE_TIME:(-0.25 0 0.808)(0 0.808 1)(0.808 1 1.25)][TRANSIT_DISTANCE:(-0.25 0 0.221)(0 0.221 1)(0.221 1 1.25)][TRANSIT_EMISSIONS:(-0.25 0 0.712)(0 0.712 1)(0.712 1 1.25)][TRANSIT_PRICE:(-0.25 0 0.657)(0 0.657 1)(0.657 1 1.25)][TRANSIT_TIME:(-0.25 0 0.318)(0 0.318 1)(0.318 1 1.25)][WALK_DISTANCE:(-0.25 0 0.992)(0 0.992 1)(0.992 1 1.25)][WALK_EMISSIONS:(-0.25 0 0.127)(0 0.127 1)(0.127 1 1.25)][WALK_PRICE:(-0.25 0 0.129)(0 0.129 1)(0.129 1 1.25)][WALK_TIME:(-0.25 0 0.596)(0 0.596 1)(0.596 1 1.25)]";

        FuzzyLogicTrainer::FuzzyExperiment exp;
        exp.fromString( expert_knowledge );
        ruleset = exp.ruleset;
    }

    // else
    return ruleset;

    /*QString best_bilbao_3 = "[AlgebraicSum,BoundedDifference,DrasticSum,HamacherProduct] if (BICYCLE_TIME is extremely HIGH)(WALK_PRICE is seldom LOW)(WALK_EMISSIONS is not MEDIUM)(TRANSIT_EMISSIONS is somewhat HIGH)(BICYCLE_TIME is extremely HIGH)(WALK_TIME is extremely MEDIUM)then BICYCLE if (WALK_PRICE is extremely LOW)(BICYCLE_DISTANCE is seldom LOW)(CAR_EMISSIONS is not HIGH)(MOTORCYCLE_EMISSIONS is seldom HIGH)(WALK_DISTANCE is not HIGH)then TRANSIT if (CAR_PRICE is not MEDIUM)(WALK_PRICE is somewhat MEDIUM)(MOTORCYCLE_TIME is not LOW)(CAR_EMISSIONS is not LOW)(MOTORCYCLE_PRICE is somewhat HIGH)(MOTORCYCLE_DISTANCE is  LOW)(BICYCLE_TIME is  MEDIUM)(BICYCLE_TIME is somewhat LOW)(TRANSIT_DISTANCE is very HIGH)(MOTORCYCLE_EMISSIONS is extremely HIGH)(WALK_TIME is not LOW)(WALK_DISTANCE is somewhat HIGH)(MOTORCYCLE_TIME is very HIGH)(BICYCLE_PRICE is seldom MEDIUM)then WALK if (CAR_TIME is somewhat LOW)(CAR_TIME is extremely LOW)(TRANSIT_EMISSIONS is extremely HIGH)(CAR_DISTANCE is not LOW)then TRANSIT if (MOTORCYCLE_TIME is extremely LOW)(BICYCLE_TIME is somewhat LOW)(MOTORCYCLE_TIME is extremely LOW)(WALK_DISTANCE is not MEDIUM)(CAR_DISTANCE is somewhat LOW)then MOTORCYCLE if (MOTORCYCLE_PRICE is not LOW)(TRANSIT_PRICE is very MEDIUM)(CAR_DISTANCE is extremely HIGH)(CAR_PRICE is seldom LOW)(BICYCLE_DISTANCE is not LOW)(TRANSIT_EMISSIONS is somewhat MEDIUM)(BICYCLE_DISTANCE is seldom LOW)then MOTORCYCLE if (MOTORCYCLE_EMISSIONS is seldom HIGH)(TRANSIT_TIME is  LOW)(CAR_DISTANCE is not MEDIUM)(CAR_EMISSIONS is somewhat LOW)(CAR_TIME is  HIGH)(MOTORCYCLE_PRICE is  MEDIUM)(CAR_TIME is very LOW)(TRANSIT_PRICE is  MEDIUM)(TRANSIT_DISTANCE is  LOW)then WALK if (MOTORCYCLE_TIME is very HIGH)(WALK_DISTANCE is extremely LOW)(CAR_PRICE is seldom MEDIUM)(TRANSIT_DISTANCE is seldom LOW)(CAR_DISTANCE is very HIGH)(CAR_PRICE is seldom MEDIUM)(BICYCLE_EMISSIONS is  LOW)(BICYCLE_PRICE is very LOW)(TRANSIT_DISTANCE is not HIGH)(BICYCLE_DISTANCE is not HIGH)(WALK_PRICE is seldom MEDIUM)(CAR_TIME is not MEDIUM)then TRANSIT if (WALK_EMISSIONS is  MEDIUM)(WALK_TIME is seldom MEDIUM)(TRANSIT_TIME is very MEDIUM)(CAR_DISTANCE is very MEDIUM)(BICYCLE_TIME is not LOW)(CAR_DISTANCE is somewhat LOW)(BICYCLE_EMISSIONS is not HIGH)(TRANSIT_PRICE is extremely HIGH)(BICYCLE_DISTANCE is  MEDIUM)(CAR_TIME is very LOW)(BICYCLE_PRICE is extremely MEDIUM)(MOTORCYCLE_DISTANCE is very LOW)then WALK if (MOTORCYCLE_EMISSIONS is somewhat HIGH)(TRANSIT_PRICE is seldom MEDIUM)(TRANSIT_TIME is  HIGH)(CAR_PRICE is somewhat LOW)(CAR_EMISSIONS is extremely LOW)(CAR_DISTANCE is very MEDIUM)(BICYCLE_EMISSIONS is extremely HIGH)(MOTORCYCLE_DISTANCE is very HIGH)(WALK_EMISSIONS is seldom HIGH)(TRANSIT_DISTANCE is  HIGH)(MOTORCYCLE_EMISSIONS is extremely LOW)(MOTORCYCLE_DISTANCE is  LOW)(CAR_PRICE is somewhat MEDIUM)(CAR_TIME is seldom MEDIUM)then MOTORCYCLE if (CAR_DISTANCE is very LOW)then MOTORCYCLE if (WALK_TIME is extremely MEDIUM)(MOTORCYCLE_PRICE is not HIGH)(CAR_EMISSIONS is very LOW)(WALK_PRICE is seldom LOW)(MOTORCYCLE_PRICE is  HIGH)(MOTORCYCLE_PRICE is somewhat HIGH)(CAR_DISTANCE is extremely MEDIUM)(WALK_DISTANCE is extremely HIGH)(TRANSIT_EMISSIONS is very MEDIUM)(BICYCLE_EMISSIONS is somewhat MEDIUM)(WALK_EMISSIONS is seldom MEDIUM)(TRANSIT_PRICE is somewhat MEDIUM)(BICYCLE_TIME is not MEDIUM)(CAR_TIME is  MEDIUM)(WALK_TIME is not HIGH)(TRANSIT_EMISSIONS is seldom HIGH)(WALK_DISTANCE is seldom LOW)(MOTORCYCLE_PRICE is not LOW)(TRANSIT_DISTANCE is not HIGH)then WALK if (BICYCLE_EMISSIONS is somewhat HIGH)(BICYCLE_EMISSIONS is extremely LOW)(BICYCLE_PRICE is not LOW)(MOTORCYCLE_EMISSIONS is  LOW)(CAR_EMISSIONS is extremely HIGH)(TRANSIT_TIME is seldom LOW)(TRANSIT_TIME is extremely MEDIUM)(CAR_DISTANCE is extremely MEDIUM)(MOTORCYCLE_PRICE is  MEDIUM)(BICYCLE_DISTANCE is very MEDIUM)(CAR_DISTANCE is somewhat LOW)(WALK_EMISSIONS is  LOW)(BICYCLE_EMISSIONS is  HIGH)(TRANSIT_PRICE is somewhat MEDIUM)(BICYCLE_DISTANCE is  HIGH)(TRANSIT_DISTANCE is somewhat HIGH)(MOTORCYCLE_TIME is extremely HIGH)(BICYCLE_DISTANCE is very LOW)then BICYCLE if (TRANSIT_DISTANCE is very LOW)(WALK_TIME is very LOW)(BICYCLE_TIME is not LOW)(CAR_DISTANCE is somewhat LOW)(BICYCLE_DISTANCE is  LOW)(MOTORCYCLE_DISTANCE is extremely HIGH)(CAR_DISTANCE is very MEDIUM)(MOTORCYCLE_PRICE is  HIGH)(BICYCLE_EMISSIONS is  LOW)(WALK_DISTANCE is somewhat LOW)(BICYCLE_DISTANCE is somewhat MEDIUM)(MOTORCYCLE_PRICE is  MEDIUM)(BICYCLE_PRICE is extremely HIGH)(CAR_TIME is extremely HIGH)(MOTORCYCLE_PRICE is  LOW)(WALK_EMISSIONS is somewhat LOW)(MOTORCYCLE_PRICE is extremely MEDIUM)(TRANSIT_DISTANCE is seldom MEDIUM)(TRANSIT_PRICE is very LOW)(MOTORCYCLE_TIME is somewhat LOW)then MOTORCYCLE [BICYCLE_DISTANCE:(-0.25 0 0.678)(0 0.678 1)(0.678 1 1.25)][BICYCLE_EMISSIONS:(-0.25 0 0.172)(0 0.172 1)(0.172 1 1.25)][BICYCLE_PRICE:(-0.25 0 0.625)(0 0.625 1)(0.625 1 1.25)][BICYCLE_TIME:(-0.25 0 0.625)(0 0.625 1)(0.625 1 1.25)][CAR_DISTANCE:(-0.25 0 0)(0 0 1)(0 1 1.25)][CAR_EMISSIONS:(-0.25 0 0.828)(0 0.828 1)(0.828 1 1.25)][CAR_PRICE:(-0.25 0 0.712)(0 0.712 1)(0.712 1 1.25)][CAR_TIME:(-0.25 0 0.367)(0 0.367 1)(0.367 1 1.25)][MOTORCYCLE_DISTANCE:(-0.25 0 0.482)(0 0.482 1)(0.482 1 1.25)][MOTORCYCLE_EMISSIONS:(-0.25 0 0.591)(0 0.591 1)(0.591 1 1.25)][MOTORCYCLE_PRICE:(-0.25 0 0.409)(0 0.409 1)(0.409 1 1.25)][MOTORCYCLE_TIME:(-0.25 0 0.059)(0 0.059 1)(0.059 1 1.25)][TRANSIT_DISTANCE:(-0.25 0 0.601)(0 0.601 1)(0.601 1 1.25)][TRANSIT_EMISSIONS:(-0.25 0 0.938)(0 0.938 1)(0.938 1 1.25)][TRANSIT_PRICE:(-0.25 0 0.327)(0 0.327 1)(0.327 1 1.25)][TRANSIT_TIME:(-0.25 0 0.553)(0 0.553 1)(0.553 1 1.25)][WALK_DISTANCE:(-0.25 0 0.941)(0 0.941 1)(0.941 1 1.25)][WALK_EMISSIONS:(-0.25 0 0.178)(0 0.178 1)(0.178 1 1.25)][WALK_PRICE:(-0.25 0 0.486)(0 0.486 1)(0.486 1 1.25)][WALK_TIME:(-0.25 0 0.819)(0 0.819 1)(0.819 1 1.25)]";
    QString worst_bilbao_3 = "[DrasticSum,HamacherProduct,NormalizedSum,HamacherProduct] if (TRANSIT_EMISSIONS is extremely MEDIUM) then WALK if (BICYCLE_TIME is not LOW) (BICYCLE_PRICE is somewhat MEDIUM) (TRANSIT_TIME is very MEDIUM) (TRANSIT_TIME is not MEDIUM) (WALK_TIME is somewhat MEDIUM) (MOTORCYCLE_EMISSIONS is not LOW) (BICYCLE_DISTANCE is  LOW) (CAR_DISTANCE is  LOW) (TRANSIT_EMISSIONS is not HIGH) (CAR_EMISSIONS is extremely LOW) (WALK_DISTANCE is  HIGH) (CAR_DISTANCE is extremely LOW) (MOTORCYCLE_DISTANCE is seldom HIGH) (TRANSIT_EMISSIONS is not LOW) (BICYCLE_TIME is not MEDIUM) (MOTORCYCLE_DISTANCE is somewhat LOW) then MOTORCYCLE if (TRANSIT_PRICE is somewhat MEDIUM) (BICYCLE_DISTANCE is not HIGH) (BICYCLE_EMISSIONS is very LOW) (CAR_EMISSIONS is  HIGH) (TRANSIT_DISTANCE is extremely LOW) (WALK_TIME is  HIGH) (WALK_DISTANCE is very LOW) then CAR if (CAR_EMISSIONS is extremely LOW) (WALK_DISTANCE is not HIGH) (MOTORCYCLE_PRICE is  MEDIUM) (MOTORCYCLE_DISTANCE is somewhat LOW) (MOTORCYCLE_TIME is somewhat MEDIUM) (BICYCLE_TIME is extremely MEDIUM) (CAR_TIME is  MEDIUM) (MOTORCYCLE_DISTANCE is very LOW) (MOTORCYCLE_PRICE is extremely MEDIUM) then CAR if (CAR_EMISSIONS is not LOW) (WALK_DISTANCE is not LOW) then TRANSIT if (CAR_TIME is  HIGH) (MOTORCYCLE_EMISSIONS is seldom MEDIUM) (BICYCLE_DISTANCE is  HIGH) (CAR_PRICE is  LOW) (TRANSIT_TIME is somewhat HIGH) (BICYCLE_EMISSIONS is seldom LOW) (WALK_EMISSIONS is very HIGH) (MOTORCYCLE_PRICE is somewhat MEDIUM) then MOTORCYCLE if (BICYCLE_TIME is extremely HIGH) (MOTORCYCLE_TIME is somewhat LOW) (CAR_TIME is extremely MEDIUM) (TRANSIT_TIME is somewhat LOW) (MOTORCYCLE_EMISSIONS is very HIGH) (BICYCLE_TIME is  HIGH) then WALK if (BICYCLE_PRICE is somewhat MEDIUM) (BICYCLE_EMISSIONS is seldom LOW) (MOTORCYCLE_EMISSIONS is seldom MEDIUM) (CAR_TIME is somewhat MEDIUM) (WALK_PRICE is seldom MEDIUM) (CAR_EMISSIONS is not LOW) (TRANSIT_PRICE is somewhat HIGH) (TRANSIT_EMISSIONS is somewhat HIGH) (MOTORCYCLE_TIME is somewhat HIGH) (CAR_TIME is  MEDIUM) (CAR_EMISSIONS is not HIGH) (BICYCLE_EMISSIONS is  LOW) (MOTORCYCLE_TIME is  MEDIUM) then TRANSIT if (TRANSIT_EMISSIONS is very MEDIUM) (WALK_DISTANCE is  HIGH) (MOTORCYCLE_EMISSIONS is seldom HIGH) (WALK_PRICE is seldom HIGH) (TRANSIT_EMISSIONS is somewhat MEDIUM) (CAR_TIME is  HIGH) (CAR_TIME is very LOW) (WALK_PRICE is somewhat MEDIUM) (TRANSIT_EMISSIONS is very MEDIUM) (MOTORCYCLE_DISTANCE is seldom MEDIUM) (MOTORCYCLE_EMISSIONS is very HIGH) (CAR_DISTANCE is somewhat LOW) then MOTORCYCLE if (MOTORCYCLE_PRICE is very HIGH) (TRANSIT_TIME is extremely HIGH) (BICYCLE_TIME is  MEDIUM) (BICYCLE_EMISSIONS is very LOW) (TRANSIT_EMISSIONS is  LOW) (BICYCLE_TIME is very HIGH) (CAR_EMISSIONS is extremely MEDIUM) (BICYCLE_PRICE is  LOW) (TRANSIT_DISTANCE is somewhat MEDIUM) (MOTORCYCLE_EMISSIONS is not HIGH) (CAR_EMISSIONS is very HIGH) (MOTORCYCLE_PRICE is seldom MEDIUM) (BICYCLE_PRICE is extremely MEDIUM) (TRANSIT_PRICE is extremely HIGH) (TRANSIT_EMISSIONS is somewhat MEDIUM) (MOTORCYCLE_TIME is  MEDIUM) (CAR_EMISSIONS is somewhat LOW) then BICYCLE if (BICYCLE_DISTANCE is somewhat HIGH) (WALK_TIME is  HIGH) (BICYCLE_TIME is extremely MEDIUM) (MOTORCYCLE_EMISSIONS is not LOW) (CAR_DISTANCE is not LOW) (CAR_PRICE is very LOW) (BICYCLE_EMISSIONS is extremely LOW) (BICYCLE_EMISSIONS is seldom HIGH) (TRANSIT_PRICE is extremely MEDIUM) (BICYCLE_EMISSIONS is somewhat MEDIUM) (MOTORCYCLE_TIME is somewhat LOW) (CAR_TIME is seldom HIGH) then BICYCLE if (TRANSIT_TIME is not MEDIUM) (CAR_TIME is seldom LOW) (BICYCLE_TIME is  HIGH) then BICYCLE if (TRANSIT_DISTANCE is  HIGH) (CAR_TIME is somewhat MEDIUM) (BICYCLE_DISTANCE is somewhat MEDIUM) (MOTORCYCLE_EMISSIONS is somewhat MEDIUM) (CAR_DISTANCE is somewhat HIGH) then MOTORCYCLE if (TRANSIT_DISTANCE is extremely LOW) (MOTORCYCLE_EMISSIONS is seldom MEDIUM) (MOTORCYCLE_PRICE is seldom HIGH) (WALK_EMISSIONS is seldom HIGH) (MOTORCYCLE_EMISSIONS is very LOW) (MOTORCYCLE_TIME is  MEDIUM) (WALK_DISTANCE is  HIGH) (CAR_DISTANCE is seldom MEDIUM) (MOTORCYCLE_TIME is seldom HIGH) (MOTORCYCLE_TIME is seldom HIGH) (WALK_EMISSIONS is not MEDIUM) (TRANSIT_DISTANCE is seldom LOW) (CAR_TIME is not MEDIUM) (BICYCLE_TIME is seldom MEDIUM) then WALK if (WALK_EMISSIONS is  LOW) (CAR_PRICE is extremely HIGH) (MOTORCYCLE_TIME is very LOW) (CAR_EMISSIONS is not LOW) (CAR_EMISSIONS is  LOW) (WALK_TIME is not LOW) (CAR_TIME is seldom HIGH) then BICYCLE if (BICYCLE_EMISSIONS is extremely LOW) (TRANSIT_PRICE is extremely MEDIUM) (WALK_DISTANCE is very LOW) (MOTORCYCLE_PRICE is very MEDIUM) then CAR if (MOTORCYCLE_TIME is somewhat HIGH) (TRANSIT_EMISSIONS is very LOW) (CAR_PRICE is not MEDIUM) (BICYCLE_TIME is seldom MEDIUM) then TRANSIT if (BICYCLE_TIME is somewhat LOW) (CAR_TIME is very HIGH) (BICYCLE_TIME is somewhat LOW) (MOTORCYCLE_DISTANCE is not LOW) (MOTORCYCLE_PRICE is somewhat LOW) (WALK_EMISSIONS is extremely HIGH) (BICYCLE_EMISSIONS is  LOW) (BICYCLE_PRICE is very HIGH) (WALK_PRICE is  LOW) (TRANSIT_PRICE is extremely LOW) then WALK if (BICYCLE_TIME is  MEDIUM) (WALK_EMISSIONS is not HIGH) (TRANSIT_TIME is seldom MEDIUM) (CAR_TIME is seldom MEDIUM) then BICYCLE if (WALK_EMISSIONS is somewhat LOW) (CAR_TIME is very LOW) (BICYCLE_PRICE is very HIGH) (TRANSIT_TIME is very LOW) (BICYCLE_DISTANCE is somewhat MEDIUM) (MOTORCYCLE_TIME is seldom MEDIUM) (MOTORCYCLE_EMISSIONS is not MEDIUM) (CAR_EMISSIONS is  HIGH) (TRANSIT_PRICE is extremely HIGH) (MOTORCYCLE_EMISSIONS is not HIGH) (WALK_PRICE is extremely HIGH) (BICYCLE_DISTANCE is somewhat LOW) (BICYCLE_DISTANCE is extremely LOW) (WALK_EMISSIONS is somewhat LOW) (WALK_EMISSIONS is not LOW) (MOTORCYCLE_EMISSIONS is somewhat LOW) then CAR if (BICYCLE_EMISSIONS is  HIGH) (BICYCLE_EMISSIONS is extremely MEDIUM) (BICYCLE_DISTANCE is somewhat LOW) (MOTORCYCLE_TIME is extremely MEDIUM) (BICYCLE_PRICE is seldom LOW) (BICYCLE_EMISSIONS is very MEDIUM) (TRANSIT_EMISSIONS is extremely HIGH) (TRANSIT_DISTANCE is very MEDIUM) (TRANSIT_DISTANCE is very MEDIUM) (MOTORCYCLE_DISTANCE is extremely LOW) (TRANSIT_TIME is extremely MEDIUM) (CAR_PRICE is not HIGH) (BICYCLE_TIME is extremely MEDIUM) (WALK_TIME is somewhat HIGH) then TRANSIT if (TRANSIT_PRICE is very HIGH) (MOTORCYCLE_EMISSIONS is very MEDIUM) (MOTORCYCLE_TIME is very MEDIUM) (BICYCLE_DISTANCE is very MEDIUM) (WALK_PRICE is extremely MEDIUM) (MOTORCYCLE_EMISSIONS is extremely HIGH) (CAR_TIME is somewhat MEDIUM) then MOTORCYCLE if (CAR_DISTANCE is somewhat MEDIUM) (CAR_TIME is  MEDIUM) (MOTORCYCLE_PRICE is  MEDIUM) (WALK_DISTANCE is seldom HIGH) (CAR_EMISSIONS is  LOW) (MOTORCYCLE_DISTANCE is  MEDIUM) (MOTORCYCLE_DISTANCE is very MEDIUM) (MOTORCYCLE_EMISSIONS is not HIGH) (BICYCLE_EMISSIONS is not MEDIUM) (MOTORCYCLE_PRICE is extremely MEDIUM) (CAR_DISTANCE is very HIGH) (MOTORCYCLE_DISTANCE is extremely LOW) (WALK_PRICE is seldom HIGH) (TRANSIT_EMISSIONS is not HIGH) then WALK if (BICYCLE_PRICE is very LOW) (MOTORCYCLE_EMISSIONS is seldom LOW) (BICYCLE_EMISSIONS is not MEDIUM) (MOTORCYCLE_TIME is not MEDIUM) (TRANSIT_TIME is seldom HIGH) (WALK_PRICE is somewhat HIGH) (CAR_TIME is extremely LOW) (TRANSIT_PRICE is extremely HIGH) then TRANSIT if (MOTORCYCLE_EMISSIONS is  MEDIUM) (CAR_TIME is not MEDIUM) (BICYCLE_DISTANCE is seldom LOW) then TRANSIT if (MOTORCYCLE_DISTANCE is not HIGH) (WALK_PRICE is not LOW) (WALK_DISTANCE is somewhat MEDIUM) (MOTORCYCLE_PRICE is seldom MEDIUM) then MOTORCYCLE";
    QString best_bilbao_5 = "[DrasticSum,Minimum,Maximum,Minimum] if (CAR_DISTANCE is very HIGH) then WALK if (BICYCLE_TIME is extremely HIGH) (BICYCLE_TIME is seldom MEDIUM) (BICYCLE_PRICE is  HIGH) (MOTORCYCLE_PRICE is not HIGH) (MOTORCYCLE_TIME is somewhat MEDIUM) (TRANSIT_PRICE is somewhat HIGH) (BICYCLE_TIME is very MEDIUM) (BICYCLE_PRICE is  HIGH) then BICYCLE if (WALK_DISTANCE is not LOW) (CAR_PRICE is seldom MEDIUM) (TRANSIT_DISTANCE is very LOW) (TRANSIT_DISTANCE is somewhat HIGH) (WALK_EMISSIONS is  LOW) (MOTORCYCLE_TIME is very MEDIUM) (CAR_PRICE is  MEDIUM) (MOTORCYCLE_PRICE is  HIGH) (MOTORCYCLE_EMISSIONS is seldom MEDIUM) (CAR_DISTANCE is somewhat LOW) (TRANSIT_TIME is not MEDIUM) (MOTORCYCLE_DISTANCE is seldom MEDIUM) (CAR_PRICE is somewhat HIGH) (BICYCLE_EMISSIONS is not MEDIUM) (TRANSIT_PRICE is somewhat HIGH) then MOTORCYCLE if (WALK_PRICE is extremely LOW) (BICYCLE_EMISSIONS is extremely MEDIUM) (WALK_DISTANCE is very MEDIUM) (WALK_TIME is somewhat LOW) (CAR_DISTANCE is  LOW) (WALK_DISTANCE is seldom LOW) (WALK_DISTANCE is  LOW) (WALK_PRICE is seldom LOW) (WALK_TIME is extremely HIGH) (CAR_DISTANCE is seldom MEDIUM) (MOTORCYCLE_EMISSIONS is not HIGH) (CAR_EMISSIONS is very HIGH) (BICYCLE_EMISSIONS is extremely HIGH) then WALK if (CAR_TIME is very LOW) (WALK_EMISSIONS is extremely HIGH) (BICYCLE_EMISSIONS is very LOW) (BICYCLE_PRICE is very HIGH) (MOTORCYCLE_TIME is seldom HIGH) (TRANSIT_TIME is very MEDIUM) then CAR if (CAR_TIME is extremely MEDIUM) then TRANSIT if (WALK_DISTANCE is seldom LOW) (BICYCLE_EMISSIONS is extremely MEDIUM) (MOTORCYCLE_PRICE is  MEDIUM) (BICYCLE_DISTANCE is not MEDIUM) (CAR_TIME is somewhat HIGH) (TRANSIT_TIME is seldom MEDIUM) (TRANSIT_PRICE is not LOW) then MOTORCYCLE if (TRANSIT_TIME is very MEDIUM) (MOTORCYCLE_PRICE is extremely LOW) (CAR_PRICE is very LOW) then CAR if (WALK_EMISSIONS is  MEDIUM) (CAR_TIME is seldom LOW) (BICYCLE_PRICE is somewhat LOW) (TRANSIT_PRICE is very HIGH) (BICYCLE_PRICE is somewhat LOW) then CAR if (MOTORCYCLE_TIME is very MEDIUM) (TRANSIT_DISTANCE is not HIGH) (TRANSIT_TIME is seldom MEDIUM) (MOTORCYCLE_PRICE is not HIGH) (TRANSIT_DISTANCE is very HIGH) then TRANSIT if (BICYCLE_EMISSIONS is somewhat LOW) (MOTORCYCLE_PRICE is  LOW) (CAR_EMISSIONS is extremely MEDIUM) (BICYCLE_TIME is somewhat MEDIUM) (CAR_EMISSIONS is somewhat HIGH) (BICYCLE_PRICE is seldom MEDIUM) (WALK_DISTANCE is extremely MEDIUM) (CAR_TIME is not HIGH) (WALK_TIME is not LOW) (CAR_EMISSIONS is  LOW) (TRANSIT_TIME is  LOW) (BICYCLE_DISTANCE is somewhat HIGH) (WALK_PRICE is not HIGH) (MOTORCYCLE_TIME is seldom HIGH) (MOTORCYCLE_EMISSIONS is somewhat MEDIUM) (CAR_EMISSIONS is very HIGH) (WALK_PRICE is extremely MEDIUM) then CAR if (TRANSIT_DISTANCE is somewhat LOW) (MOTORCYCLE_TIME is somewhat MEDIUM) (CAR_EMISSIONS is  MEDIUM) (CAR_EMISSIONS is somewhat HIGH) (TRANSIT_DISTANCE is very LOW) (WALK_EMISSIONS is seldom LOW) (WALK_EMISSIONS is seldom MEDIUM) (WALK_PRICE is  HIGH) (CAR_DISTANCE is extremely MEDIUM) (TRANSIT_EMISSIONS is very HIGH) then CAR if (BICYCLE_EMISSIONS is very HIGH) (MOTORCYCLE_TIME is not HIGH) then BICYCLE if (BICYCLE_PRICE is seldom LOW) (MOTORCYCLE_TIME is not LOW) (BICYCLE_TIME is  MEDIUM) (CAR_DISTANCE is very HIGH) (MOTORCYCLE_TIME is extremely LOW) (CAR_PRICE is extremely MEDIUM) (BICYCLE_DISTANCE is seldom HIGH) (BICYCLE_PRICE is extremely MEDIUM) then WALK";
    QString worst_bilbao_5 = "[Maximum,EinsteinProduct,NormalizedSum,EinsteinProduct] if (CAR_DISTANCE is very HIGH)then WALK if (BICYCLE_TIME is extremely HIGH)(BICYCLE_TIME is seldom MEDIUM)(BICYCLE_PRICE is  HIGH)(MOTORCYCLE_PRICE is not HIGH)(MOTORCYCLE_TIME is somewhat MEDIUM)(TRANSIT_PRICE is somewhat HIGH)(BICYCLE_TIME is very MEDIUM)then BICYCLE if (WALK_DISTANCE is not LOW)(CAR_PRICE is seldom MEDIUM)(TRANSIT_DISTANCE is very LOW)(TRANSIT_DISTANCE is somewhat HIGH)(WALK_EMISSIONS is  LOW)(MOTORCYCLE_TIME is very MEDIUM)(CAR_PRICE is  MEDIUM)(MOTORCYCLE_PRICE is  HIGH)(MOTORCYCLE_EMISSIONS is seldom MEDIUM)(CAR_DISTANCE is somewhat LOW)(TRANSIT_TIME is not MEDIUM)(MOTORCYCLE_DISTANCE is seldom MEDIUM)(CAR_PRICE is somewhat HIGH)(BICYCLE_EMISSIONS is not MEDIUM)(TRANSIT_PRICE is somewhat HIGH)then MOTORCYCLE if (WALK_PRICE is extremely LOW)(BICYCLE_EMISSIONS is extremely MEDIUM)(WALK_DISTANCE is very MEDIUM)(WALK_TIME is somewhat LOW)(CAR_DISTANCE is  LOW)(WALK_DISTANCE is seldom LOW)(WALK_DISTANCE is  LOW)(WALK_PRICE is seldom LOW)(WALK_TIME is extremely HIGH)(CAR_DISTANCE is seldom MEDIUM)(MOTORCYCLE_EMISSIONS is not HIGH)(CAR_EMISSIONS is very HIGH)(BICYCLE_EMISSIONS is extremely HIGH)then WALK if (CAR_TIME is very LOW)(WALK_EMISSIONS is extremely HIGH)(BICYCLE_EMISSIONS is very LOW)(BICYCLE_PRICE is very HIGH)(MOTORCYCLE_TIME is seldom HIGH)(TRANSIT_TIME is very MEDIUM)then CAR if (CAR_TIME is extremely MEDIUM)then TRANSIT if (WALK_DISTANCE is seldom LOW)(BICYCLE_EMISSIONS is extremely MEDIUM)(MOTORCYCLE_PRICE is  MEDIUM)(BICYCLE_DISTANCE is not MEDIUM)(CAR_TIME is somewhat HIGH)(TRANSIT_TIME is seldom MEDIUM)(TRANSIT_PRICE is not LOW)then MOTORCYCLE if (TRANSIT_TIME is very MEDIUM)(MOTORCYCLE_PRICE is extremely LOW)(CAR_PRICE is very LOW)then CAR if (WALK_EMISSIONS is  MEDIUM)(CAR_TIME is seldom LOW)(BICYCLE_PRICE is somewhat LOW)(TRANSIT_PRICE is very HIGH)then CAR if (MOTORCYCLE_TIME is very MEDIUM)(TRANSIT_DISTANCE is not HIGH)(TRANSIT_TIME is seldom MEDIUM)(MOTORCYCLE_PRICE is not HIGH)(TRANSIT_DISTANCE is very HIGH)then TRANSIT if (BICYCLE_EMISSIONS is somewhat LOW)(MOTORCYCLE_PRICE is  LOW)(CAR_EMISSIONS is extremely MEDIUM)(BICYCLE_TIME is somewhat MEDIUM)(CAR_EMISSIONS is somewhat HIGH)(BICYCLE_PRICE is seldom MEDIUM)(WALK_DISTANCE is extremely MEDIUM)(CAR_TIME is not HIGH)(WALK_TIME is not LOW)(CAR_EMISSIONS is  LOW)(TRANSIT_TIME is  LOW)(BICYCLE_DISTANCE is somewhat HIGH)(WALK_PRICE is not HIGH)(MOTORCYCLE_TIME is seldom HIGH)(MOTORCYCLE_EMISSIONS is somewhat MEDIUM)(CAR_EMISSIONS is very HIGH)(WALK_PRICE is extremely MEDIUM)then CAR if (TRANSIT_DISTANCE is somewhat LOW)(MOTORCYCLE_TIME is somewhat MEDIUM)(CAR_EMISSIONS is  MEDIUM)(CAR_EMISSIONS is somewhat HIGH)(TRANSIT_DISTANCE is very LOW)(WALK_EMISSIONS is seldom LOW)(WALK_EMISSIONS is seldom MEDIUM)(WALK_PRICE is  HIGH)(CAR_DISTANCE is extremely MEDIUM)(TRANSIT_EMISSIONS is very HIGH)then CAR if (BICYCLE_EMISSIONS is very HIGH)(MOTORCYCLE_TIME is not HIGH)then BICYCLE if (BICYCLE_PRICE is seldom LOW)(MOTORCYCLE_TIME is not LOW)(BICYCLE_TIME is  MEDIUM)(CAR_DISTANCE is very HIGH)(MOTORCYCLE_TIME is extremely LOW)(CAR_PRICE is extremely MEDIUM)(BICYCLE_DISTANCE is seldom HIGH)(BICYCLE_PRICE is extremely MEDIUM)then WALK [BICYCLE_DISTANCE:(-0.25 0 0.427)(0 0.427 1)(0.427 1 1.25)][BICYCLE_EMISSIONS:(-0.25 0 0.062)(0 0.062 1)(0.062 1 1.25)][BICYCLE_PRICE:(-0.25 0 0.564)(0 0.564 1)(0.564 1 1.25)][BICYCLE_TIME:(-0.25 0 0.463)(0 0.463 1)(0.463 1 1.25)][CAR_DISTANCE:(-0.25 0 0.928)(0 0.928 1)(0.928 1 1.25)][CAR_EMISSIONS:(-0.25 0 0.167)(0 0.167 1)(0.167 1 1.25)][CAR_PRICE:(-0.25 0 0.305)(0 0.305 1)(0.305 1 1.25)][CAR_TIME:(-0.25 0 0.661)(0 0.661 1)(0.661 1 1.25)][MOTORCYCLE_DISTANCE:(-0.25 0 0.2)(0 0.2 1)(0.2 1 1.25)][MOTORCYCLE_EMISSIONS:(-0.25 0 0.656)(0 0.656 1)(0.656 1 1.25)][MOTORCYCLE_PRICE:(-0.25 0 0.993)(0 0.993 1)(0.993 1 1.25)][MOTORCYCLE_TIME:(-0.25 0 0.355)(0 0.355 1)(0.355 1 1.25)][TRANSIT_DISTANCE:(-0.25 0 0.11)(0 0.11 1)(0.11 1 1.25)][TRANSIT_EMISSIONS:(-0.25 0 0.704)(0 0.704 1)(0.704 1 1.25)][TRANSIT_PRICE:(-0.25 0 0.442)(0 0.442 1)(0.442 1 1.25)][TRANSIT_TIME:(-0.25 0 0.315)(0 0.315 1)(0.315 1 1.25)][WALK_DISTANCE:(-0.25 0 0.977)(0 0.977 1)(0.977 1 1.25)][WALK_EMISSIONS:(-0.25 0 0.975)(0 0.975 1)(0.975 1 1.25)][WALK_PRICE:(-0.25 0 0.144)(0 0.144 1)(0.144 1 1.25)][WALK_TIME:(-0.25 0 0.312)(0 0.312 1)(0.312 1 1.25)]";
    QString best_silesia_3;
    QString worst_silesia_3 = "[HamacherSum,AlgebraicProduct,AlgebraicSum,Minimum] if (WALK_DISTANCE is not MEDIUM)(TRANSIT_PRICE is very LOW)(CAR_PRICE is very MEDIUM)(TRANSIT_EMISSIONS is seldom HIGH)(BICYCLE_DISTANCE is somewhat HIGH)(BICYCLE_DISTANCE is extremely MEDIUM)(ELECTRIC_DISTANCE is very LOW)(MOTORCYCLE_PRICE is  MEDIUM)(TRANSIT_EMISSIONS is not MEDIUM)(MOTORCYCLE_EMISSIONS is not HIGH)(CAR_PRICE is  MEDIUM)(TRANSIT_PRICE is  LOW)(MOTORCYCLE_DISTANCE is very MEDIUM)(BICYCLE_TIME is  MEDIUM)then TRANSIT if (BICYCLE_EMISSIONS is  LOW)(ELECTRIC_DISTANCE is very LOW)(ELECTRIC_DISTANCE is somewhat HIGH)(TRANSIT_EMISSIONS is very HIGH)(ELECTRIC_DISTANCE is very LOW)(BICYCLE_DISTANCE is seldom HIGH)then BICYCLE if (TRANSIT_DISTANCE is very HIGH)then TRANSIT if (TRANSIT_EMISSIONS is extremely MEDIUM)(TRANSIT_TIME is seldom HIGH)(CAR_PRICE is extremely LOW)(CAR_PRICE is somewhat HIGH)(BICYCLE_DISTANCE is  HIGH)(CAR_PRICE is extremely LOW)(BICYCLE_PRICE is somewhat MEDIUM)(ELECTRIC_DISTANCE is not MEDIUM)(WALK_PRICE is extremely MEDIUM)(WALK_PRICE is somewhat LOW)(WALK_EMISSIONS is somewhat LOW)(WALK_PRICE is extremely HIGH)(WALK_EMISSIONS is somewhat MEDIUM)(CAR_EMISSIONS is very HIGH)(CAR_DISTANCE is somewhat LOW)(MOTORCYCLE_TIME is somewhat HIGH)then TRANSIT if (ELECTRIC_EMISSIONS is not MEDIUM)(MOTORCYCLE_TIME is extremely MEDIUM)(ELECTRIC_DISTANCE is not MEDIUM)(MOTORCYCLE_PRICE is not HIGH)(BICYCLE_PRICE is very MEDIUM)(MOTORCYCLE_DISTANCE is somewhat MEDIUM)(CAR_DISTANCE is somewhat LOW)(MOTORCYCLE_PRICE is very MEDIUM)(BICYCLE_DISTANCE is seldom HIGH)(WALK_TIME is very HIGH)(CAR_DISTANCE is extremely MEDIUM)(ELECTRIC_EMISSIONS is somewhat HIGH)(WALK_EMISSIONS is not LOW)then CAR if (ELECTRIC_EMISSIONS is seldom HIGH)(MOTORCYCLE_EMISSIONS is somewhat MEDIUM)(CAR_TIME is somewhat HIGH)(TRANSIT_PRICE is somewhat LOW)(WALK_DISTANCE is not LOW)(MOTORCYCLE_TIME is seldom MEDIUM)then BICYCLE if (ELECTRIC_TIME is somewhat HIGH)(WALK_TIME is not LOW)(CAR_TIME is very HIGH)(CAR_DISTANCE is extremely LOW)(BICYCLE_DISTANCE is very MEDIUM)then ELECTRIC if (WALK_TIME is somewhat HIGH)(TRANSIT_TIME is not MEDIUM)then WALK if (TRANSIT_DISTANCE is seldom HIGH)(TRANSIT_PRICE is extremely LOW)(BICYCLE_PRICE is extremely HIGH)(BICYCLE_DISTANCE is  MEDIUM)(BICYCLE_TIME is seldom MEDIUM)(MOTORCYCLE_EMISSIONS is extremely HIGH)(MOTORCYCLE_PRICE is not MEDIUM)(WALK_TIME is somewhat LOW)(CAR_EMISSIONS is somewhat MEDIUM)(MOTORCYCLE_PRICE is  HIGH)(TRANSIT_TIME is not LOW)(WALK_DISTANCE is not MEDIUM)(ELECTRIC_DISTANCE is seldom LOW)(WALK_TIME is very MEDIUM)(CAR_EMISSIONS is very HIGH)(BICYCLE_EMISSIONS is  MEDIUM)(ELECTRIC_DISTANCE is  HIGH)(CAR_DISTANCE is extremely MEDIUM)then CAR if (MOTORCYCLE_EMISSIONS is somewhat HIGH)(BICYCLE_PRICE is not MEDIUM)(BICYCLE_TIME is somewhat MEDIUM)(MOTORCYCLE_DISTANCE is somewhat MEDIUM)(CAR_DISTANCE is somewhat HIGH)(WALK_TIME is very MEDIUM)(WALK_TIME is  LOW)(TRANSIT_EMISSIONS is extremely HIGH)(WALK_PRICE is seldom MEDIUM)(MOTORCYCLE_PRICE is not HIGH)(CAR_DISTANCE is  LOW)(TRANSIT_TIME is seldom HIGH)(BICYCLE_DISTANCE is extremely LOW)(CAR_DISTANCE is not MEDIUM)(TRANSIT_DISTANCE is not MEDIUM)(WALK_EMISSIONS is somewhat HIGH)(CAR_PRICE is somewhat MEDIUM)(CAR_DISTANCE is very MEDIUM)then MOTORCYCLE if (BICYCLE_TIME is somewhat LOW)(MOTORCYCLE_DISTANCE is  LOW)(CAR_EMISSIONS is very HIGH)(WALK_DISTANCE is not MEDIUM)(MOTORCYCLE_TIME is  HIGH)(BICYCLE_DISTANCE is very MEDIUM)(CAR_TIME is very LOW)(MOTORCYCLE_TIME is  HIGH)(BICYCLE_TIME is extremely HIGH)(BICYCLE_EMISSIONS is very HIGH)(WALK_DISTANCE is extremely HIGH)(TRANSIT_TIME is not HIGH)(TRANSIT_TIME is very LOW)(MOTORCYCLE_DISTANCE is seldom LOW)(WALK_TIME is  MEDIUM)(ELECTRIC_TIME is seldom HIGH)(WALK_TIME is somewhat LOW)then ELECTRIC if (CAR_PRICE is seldom MEDIUM)(TRANSIT_DISTANCE is  MEDIUM)(BICYCLE_PRICE is extremely LOW)(CAR_PRICE is somewhat HIGH)(MOTORCYCLE_TIME is  LOW)then ELECTRIC if (ELECTRIC_TIME is extremely LOW)(MOTORCYCLE_TIME is somewhat HIGH)(BICYCLE_PRICE is seldom HIGH)(ELECTRIC_TIME is not LOW)(CAR_DISTANCE is  LOW)(ELECTRIC_DISTANCE is very LOW)(MOTORCYCLE_TIME is very HIGH)(WALK_PRICE is extremely LOW)(WALK_DISTANCE is very LOW)(CAR_DISTANCE is  HIGH)(MOTORCYCLE_TIME is seldom MEDIUM)(CAR_DISTANCE is  LOW)(CAR_DISTANCE is somewhat HIGH)(ELECTRIC_PRICE is  HIGH)(ELECTRIC_EMISSIONS is seldom HIGH)(ELECTRIC_PRICE is somewhat LOW)then MOTORCYCLE [BICYCLE_DISTANCE:(-0.25 0 0.452)(0 0.452 1)(0.452 1 1.25)][BICYCLE_EMISSIONS:(-0.25 0 0.399)(0 0.399 1)(0.399 1 1.25)][BICYCLE_PRICE:(-0.25 0 0.882)(0 0.882 1)(0.882 1 1.25)][BICYCLE_TIME:(-0.25 0 0.088)(0 0.088 1)(0.088 1 1.25)][CAR_DISTANCE:(-0.25 0 0.159)(0 0.159 1)(0.159 1 1.25)][CAR_EMISSIONS:(-0.25 0 0.006)(0 0.006 1)(0.006 1 1.25)][CAR_PRICE:(-0.25 0 0.57)(0 0.57 1)(0.57 1 1.25)][CAR_TIME:(-0.25 0 0.826)(0 0.826 1)(0.826 1 1.25)][ELECTRIC_DISTANCE:(-0.25 0 0.562)(0 0.562 1)(0.562 1 1.25)][ELECTRIC_EMISSIONS:(-0.25 0 0.963)(0 0.963 1)(0.963 1 1.25)][ELECTRIC_PRICE:(-0.25 0 0.003)(0 0.003 1)(0.003 1 1.25)][ELECTRIC_TIME:(-0.25 0 0.433)(0 0.433 1)(0.433 1 1.25)][MOTORCYCLE_DISTANCE:(-0.25 0 0.406)(0 0.406 1)(0.406 1 1.25)][MOTORCYCLE_EMISSIONS:(-0.25 0 0.862)(0 0.862 1)(0.862 1 1.25)][MOTORCYCLE_PRICE:(-0.25 0 0.681)(0 0.681 1)(0.681 1 1.25)][MOTORCYCLE_TIME:(-0.25 0 0.657)(0 0.657 1)(0.657 1 1.25)][TRANSIT_DISTANCE:(-0.25 0 0.789)(0 0.789 1)(0.789 1 1.25)][TRANSIT_EMISSIONS:(-0.25 0 0.38)(0 0.38 1)(0.38 1 1.25)][TRANSIT_PRICE:(-0.25 0 0.266)(0 0.266 1)(0.266 1 1.25)][TRANSIT_TIME:(-0.25 0 0.229)(0 0.229 1)(0.229 1 1.25)][WALK_DISTANCE:(-0.25 0 0.473)(0 0.473 1)(0.473 1 1.25)][WALK_EMISSIONS:(-0.25 0 0.316)(0 0.316 1)(0.316 1 1.25)][WALK_PRICE:(-0.25 0 0.625)(0 0.625 1)(0.625 1 1.25)][WALK_TIME:(-0.25 0 0.038)(0 0.038 1)(0.038 1 1.25)]";
    QString best_silesia_5;
    QString worst_silesia_5;*/

}

FuzzyLogic::Rule FuzzyLogicTrainer::generateRule(){

    FuzzyLogic::Rule rule;
    int terms_amount = (qrand() % 20)+1;
    for( int k = 0 ; k < terms_amount ; k++ ){
        rule.terms.append( this->generateRuleTerm() );
    }
    rule.output_value = this->output_names.at( qrand() % this->output_names.size() );
    return rule;
}

FuzzyLogic::RuleTerm FuzzyLogicTrainer::generateRuleTerm(){
    FuzzyLogic::RuleTerm term;
    term.input_name = this->input_names.keys().at( qrand() % this->input_names.keys().size() );
    QStringList hedges; hedges << "" << "not" << "somewhat" << "seldom" << "very" << "extremely";
    term.hedge = hedges.at( qrand() % hedges.size() );
    term.term_name = this->input_names.value( term.input_name ).at( qrand() % this->input_names.value( term.input_name ).size() );
    return term;
}

FuzzyLogic::OutputMembershipFunction FuzzyLogicTrainer::generateOutput(){

    FuzzyLogic::OutputMembershipFunction output;

    output.output_name = "TRANSPORT";
    foreach(QString output_name , this->output_names ){
        FuzzyLogic::MemberShipFunctionTerm term;
        term.name = output_name;
        term.shape_points.append( this->output_names.indexOf( output_name ) );
        term.shape_points.append( this->output_names.indexOf( output_name )+1 );
        term.shape_height = 10;
        output.terms.append( term );
    }

    return output;
}

/**********************************************************************
   EXECUTION
**********************************************************************/

void FuzzyLogicTrainer::init(){

    this->best_fitness = 0;

    // Create randomly best fuzzy experiment
    this->best_experiment.inputset = this->generateInputset();
    this->best_experiment.ruleset = this->generateRuleset();
    this->best_experiment.output = this->generateOutput();
    this->best_experiment.accumulator = this->accumulators.at( qrand() % this->accumulators.size() );
    this->best_experiment.conjunction = this->conjunctions.at( qrand() % this->conjunctions.size() );
    this->best_experiment.disjunction = this->disjunctions.at( qrand() % this->disjunctions.size() );
    this->best_experiment.activator = this->activators.at( qrand() % this->activators.size() );
    this->best_fitness = this->calculateFitness( this->best_experiment.inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );

    // Generate population
    while( this->rule_population.size() < this->population_size ){
        QList< FuzzyLogic::Rule > ruleset = this->generateRuleset();
        this->rule_population.append( ruleset );
        this->rule_polulation_fitness.append( this->calculateFitness( this->best_experiment.inputset , ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator ) );
    }

    while( this->input_population.size() < this->population_size ){
        QList< FuzzyLogic::InputMemberShipFunction > inputset = this->generateInputset();
        this->input_population.append( inputset );
        this->input_population_fitness.append( this->calculateFitness( inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator ) );
    }

    // Add crossover operators
    this->rule_operators.append( new RuleCombineCrossover( this ) );
    this->rule_operators.append( new RuleSliceCrossover( this ) );
    this->rule_operators.append( new RuleSliceTermsCrossover( this ) );
    this->input_operators.append( new InputCombineCrossover( this ) );

    // Add mutation operators
    /*this->rule_operators.append( new RuleAddMutation( this ) );
    this->rule_operators.append( new RuleAddTermMutation( this ) );
    this->rule_operators.append( new RuleOutputMutation( this ) );
    this->rule_operators.append( new RuleRemoveMutation( this ) );
    this->rule_operators.append( new RuleRemoveTermMutation( this ) );
    this->rule_operators.append( new RuleTermInputMutation( this ) );
    this->rule_operators.append( new RuleTermHedgeMutation( this ) );
    this->rule_operators.append( new RuleTermVariableMutation( this ) );*/
    this->input_operators.append( new InputShapeMutation( this ) );

}

void FuzzyLogicTrainer::execute(){

    // LOOP
    for( this->iteration = 0 ; this->iteration < 999999 ; this->iteration++ ){

        // Each loop make some stuff
        this->loop();
    }

    // Set some finish params
    this->finish();
}

void FuzzyLogicTrainer::loop(){

    // Change rules for const best input and const output
    this->executeRules();

    // Change inputs for const best rules and const output
    this->executeInputs();

    // Change accumulators
    this->executeAccumulators();

    // Change conjunctions
    this->executeConjunctions();

    // Change disjunctions
    this->executeDisjunctions();

    // Change activators
    this->executeActivators();

}

void FuzzyLogicTrainer::finish(){

    qInfo() << QString("EXECUTION FINISHED");

    // Inputs
    foreach( FuzzyLogic::InputMemberShipFunction i , this->best_experiment.inputset ){
        foreach(FuzzyLogic::MemberShipFunctionTerm t , i.terms ){
            qInfo() << QString("%1 : %2 (%3 , %4 , %5)").arg( i.input_name ).arg( t.name ).arg( t.shape_points.at(0) ).arg( t.shape_points.at(1) ).arg( t.shape_points.at(2) );
        }
    }

    // Rules
    foreach( FuzzyLogic::Rule r , this->best_experiment.ruleset ){
        QString str = QString("then %1").arg( r.output_value );
        foreach(FuzzyLogic::RuleTerm t , r.terms ){
            str.prepend( QString("%1 IS %2 %3 ").arg( t.input_name ).arg( t.hedge ).arg( t.term_name) );
        }
        qInfo() << str;
    }
}

/**********************************************************************
   EXECUTION LOOP INSIDE
**********************************************************************/

void FuzzyLogicTrainer::executeRules(){

    // Create new best fitness of this iteration
    QList< QList< FuzzyLogic::Rule> > new_rule_population = QList< QList< FuzzyLogic::Rule> >();
    QList< double > new_rule_population_fitness = QList<double>();
    double new_best_fitness = 0;
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // Store best from last iteration
    QList< FuzzyLogic::Rule > old_best_ruleset = this->best_experiment.ruleset;

    if( !this->rule_operators.isEmpty() ){
        for( this->subiteration = new_rule_population.size() ; this->subiteration < this->rule_population.size()-1 ; this->subiteration++ ){

            FuzzyLogicTrainerOperator* t = this->rule_operators.at( qrand() % this->rule_operators.size() );
            QList< FuzzyLogic::Rule > candidate_ruleset;

            if( RuleMutationOperator* mutation = dynamic_cast< RuleMutationOperator* >( t ) ){
                mutation->init( this->rule_population.at( this->subiteration ) );
                mutation->run();
                candidate_ruleset = mutation->getObtainedRuleset();
            }
            if( RuleCrossoverOperator* crossover = dynamic_cast< RuleCrossoverOperator* >( t ) ){
                crossover->init( this->rule_population.at( this->subiteration ) , this->rule_population.at( ((this->subiteration+1)%this->rule_population.size()) ) );
                crossover->run();
                candidate_ruleset = crossover->getObtainedRuleset();
            }

            double candidate_fitness = t->getObtainedFitness();
            if( !new_rule_population_fitness.contains( candidate_fitness ) ){
                new_rule_population.append( candidate_ruleset );
                new_rule_population_fitness.append( candidate_fitness );
                if( candidate_fitness >= new_best_fitness ){
                    this->best_experiment.ruleset = candidate_ruleset;
                    new_best_fitness = this->best_fitness = candidate_fitness;
                }
            }

        }
    }

    // Test best from last iteration
    {
        double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , old_best_ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
        if( candidate_fitness >= new_best_fitness ){
            this->best_experiment.ruleset = old_best_ruleset;
            new_best_fitness = this->best_fitness = candidate_fitness;
        }
    }

    // Add new individuals rule population
    while( new_rule_population.size() < this->rule_population.size() ){
        QList< FuzzyLogic::Rule > ruleset = this->generateRuleset();
        double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
        new_rule_population.append( ruleset );
        new_rule_population_fitness.append( candidate_fitness );
        if( candidate_fitness >= new_best_fitness ){
            this->best_experiment.ruleset = ruleset;
            new_best_fitness = this->best_fitness = candidate_fitness;
        }
    }

    // Update population
    this->rule_population = new_rule_population;
    this->rule_polulation_fitness = new_rule_population_fitness;
}


void FuzzyLogicTrainer::executeInputs(){

    // Create new best fitness of this iteration
    QList< QList< FuzzyLogic::InputMemberShipFunction> > new_input_population = QList< QList< FuzzyLogic::InputMemberShipFunction> >();
    QList< double > new_input_population_fitness = QList<double>();
    double new_best_fitness = 0;
    qsrand( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    // Store best from last iteration
    QList< FuzzyLogic::InputMemberShipFunction > old_best_inputset = this->best_experiment.inputset;

    // Store runnables being executed in another thread
    QList<FuzzyLogicTrainerOperator*> pending_operators = QList<FuzzyLogicTrainerOperator*>();

    if( !this->input_operators.isEmpty() ){
        for( this->subiteration = new_input_population.size() ; this->subiteration < this->input_population.size()-1 ; this->subiteration++ ){

            FuzzyLogicTrainerOperator* t = this->input_operators.at( qrand() % this->input_operators.size() );
            QList< FuzzyLogic::InputMemberShipFunction > candidate_inputset;

            if( InputMutationOperator* mutation = dynamic_cast< InputMutationOperator* >( t ) ){
                mutation->init( this->input_population.at( this->subiteration ) );
                mutation->run();
                candidate_inputset = mutation->getObtainedInputset();
            }
            if( InputCrossoverOperator* crossover = dynamic_cast< InputCrossoverOperator* >( t ) ){
                crossover->init( this->input_population.at( this->subiteration ) , this->input_population.at( ((this->subiteration+1)%this->input_population.size()) ) );
                crossover->run();
                candidate_inputset = crossover->getObtainedInputset();
            }

            double candidate_fitness = t->getObtainedFitness();
            if( !new_input_population_fitness.contains( candidate_fitness ) ){
                new_input_population.append( candidate_inputset );
                new_input_population_fitness.append( candidate_fitness );
                if( candidate_fitness >= new_best_fitness ){
                    this->best_experiment.inputset = candidate_inputset;
                    new_best_fitness = this->best_fitness = candidate_fitness;
                }
            }

        }
    }


    // Test best from last iteration
    {
        double candidate_fitness = this->calculateFitness( old_best_inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
        if( candidate_fitness >= new_best_fitness ){
            this->best_experiment.inputset = old_best_inputset;
            new_best_fitness = this->best_fitness = candidate_fitness;
        }
    }

    // Add new individuals rule population
    while( new_input_population.size() < this->input_population.size() ){
        QList< FuzzyLogic::InputMemberShipFunction > inputset = this->generateInputset();
        double candidate_fitness = this->calculateFitness( inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
        new_input_population.append( inputset );
        new_input_population_fitness.append( candidate_fitness );
        if( candidate_fitness >= new_best_fitness ){
            this->best_experiment.inputset = inputset;
            new_best_fitness = this->best_fitness = candidate_fitness;
        }
    }

    // Update population
    this->input_population = new_input_population;
    this->input_population_fitness = new_input_population_fitness;
}

void FuzzyLogicTrainer::executeAccumulators(){

    QString candidate_accumulator = this->accumulators.at( qrand() % this->accumulators.size() );

    // Compare the new candidate ruleset with the best
    double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , this->best_experiment.ruleset , this->best_experiment.output , candidate_accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
    if( candidate_fitness >= this->best_fitness ){
        this->best_experiment.accumulator = candidate_accumulator;
        this->best_fitness = candidate_fitness;
    }
}

void FuzzyLogicTrainer::executeConjunctions(){

    QString candidate_conjunction = this->conjunctions.at( qrand() % this->conjunctions.size() );

    // Compare the new candidate ruleset with the best
    double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , candidate_conjunction , this->best_experiment.disjunction , this->best_experiment.activator );
    if( candidate_fitness >= this->best_fitness ){
        this->best_experiment.conjunction = candidate_conjunction;
        this->best_fitness = candidate_fitness;
    }
}

void FuzzyLogicTrainer::executeDisjunctions(){

    QString candidate_disjunction = this->disjunctions.at( qrand() % this->disjunctions.size() );

    // Compare the new candidate ruleset with the best
    double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , candidate_disjunction , this->best_experiment.activator );
    if( candidate_fitness >= this->best_fitness ){
        this->best_experiment.disjunction = candidate_disjunction;
        this->best_fitness = candidate_fitness;
    }
}

void FuzzyLogicTrainer::executeActivators(){

    QString candidate_activator = this->activators.at( qrand() % this->activators.size() );

    // Compare the new candidate ruleset with the best
    double candidate_fitness = this->calculateFitness( this->best_experiment.inputset , this->best_experiment.ruleset , this->best_experiment.output , this->best_experiment.accumulator , this->best_experiment.conjunction , this->best_experiment.disjunction , candidate_activator );
    if( candidate_fitness >= this->best_fitness ){
        this->best_experiment.activator = candidate_activator;
        this->best_fitness = candidate_fitness;
    }
}

/**********************************************************************
   METHODS
**********************************************************************/

QList<FuzzyLogic::Rule> FuzzyLogicTrainer::getBestRuleset(QList<QList<FuzzyLogic::Rule> > rulesets, QVector<double> &return_fitness ){

    QList<FuzzyLogic::Rule> best_ruleset;

    // Create FuzzyExperiments and calculate their fitness
    QList<FuzzyLogicTrainer::FuzzyExperiment> experiments;
    return_fitness.clear();

    foreach( QList<FuzzyLogic::Rule> rs , rulesets ){
        FuzzyLogicTrainer::FuzzyExperiment exp = this->best_experiment;
        exp.ruleset = rs;

        experiments.append( exp );
        return_fitness.append( this->calculateFitness( exp.inputset , exp.ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator ) );
    }

    // Get Best
    int best_experiment_index = 0;
    for( int i = 0 ; i < return_fitness.size() ; i++ ){
        if( return_fitness.at( i ) > return_fitness.at( best_experiment_index ) ){
            best_experiment_index = i;
        }
    }

    best_ruleset = experiments.at( best_experiment_index ).ruleset;
    return best_ruleset;
}

QList< FuzzyLogic::InputMemberShipFunction > FuzzyLogicTrainer::getBestInput(QList<QList<FuzzyLogic::InputMemberShipFunction> > inputs , QVector<double> &return_fitness ){

    QList<FuzzyLogic::InputMemberShipFunction> best_input;

    // Create FuzzyExperiments and calculate their fitness
    QList<FuzzyLogicTrainer::FuzzyExperiment> experiments;
    return_fitness.clear();

    foreach( QList<FuzzyLogic::InputMemberShipFunction> is , inputs ){
        FuzzyLogicTrainer::FuzzyExperiment exp = this->best_experiment;
        exp.inputset = is;

        experiments.append( exp );
        return_fitness.append( this->calculateFitness( exp.inputset , exp.ruleset , exp.output , exp.accumulator , exp.conjunction , exp.disjunction , exp.activator ) );
    }

    // Get Best
    int best_experiment_index = 0;
    for( int i = 0 ; i < return_fitness.size() ; i++ ){
        if( return_fitness.at( i ) > return_fitness.at( best_experiment_index ) ){
            best_experiment_index = i;
        }
    }

    best_input = experiments.at( best_experiment_index ).inputset;
    return best_input;
}



